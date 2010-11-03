/*
 * Copyright (c) Tver Regional Scientific Library
 * Author: Alexander Fronkin
 *
 * Version 1.1 (1 Jan 2003)
 */

#ifndef __MARCREC_H
#define __MARCREC_H

#include <stdio.h>

#if defined(sun) || defined(__sun) || defined(_sun_) || defined(__solaris__)
#define _XPG6
#endif // defined(sun) || defined(__sun) || defined(_sun_) || defined(__solaris__)
#include <iconv.h>

#pragma warning (disable : 4786)
#include <string>
#include <list>
#include <iterator>

#define MARCREC_SUCCESS	1
#define MARCREC_ERROR	0
#define MARCREC_ERROR_WRONGARGS	-1
#define MARCREC_ERROR_BADRECORD	-2
#define MARCREC_ERROR_FILE		-3
#define MARCREC_ERROR_NOTFOUND	-4
#define MARCREC_ERROR_EOF		-5

#define MARCREC_SYMBOL_IS1	'\x1f'
#define MARCREC_SYMBOL_IS2	'\x1e'
#define MARCREC_SYMBOL_IS3	'\x1d'

#define MARCREC_SIZE_RECORD		100000
#define MARCREC_SIZE_FIELD		10000
#define MARCREC_SIZE_LABEL		3
#define MARCREC_SIZE_FIELDLEN	4
#define MARCREC_SIZE_OFFSETLEN	5
#define MARCREC_SIZE_RECLEN		5
#define MARCREC_SIZE_IND		2
#define MARCREC_SIZE_SUBLABEL	1

#ifndef INLINE
#define INLINE	__inline
#endif

// -----------------
// Класс Marc-записи
// -----------------
class CMarcRec {
public:
	struct TField;

	// Список полей
	typedef std::list<TField> TFieldList;
	typedef TFieldList::iterator TFieldRef;

	// Подполе
	struct TSubField {
		char cLabel;			// Метка подполя
		std::string strValue;	// Значение подполя
		TFieldRef InsFieldRef;	// Встроенное поле
	};

	// Список подполей
	typedef std::list<TSubField> TSubFieldList;
	typedef TSubFieldList::iterator TSubFieldRef;

	// Поле
	struct TField {
		unsigned short iLabel;			// Метка поля
		char szInd[MARCREC_SIZE_IND];	// Индикаторы поля
		TSubFieldList SubFieldList;		// Список подполей
	};

	#pragma pack(1)

	// Элемент списка полей записи
	struct TMapItem {
		char szLabel[MARCREC_SIZE_LABEL];		// Метка поля
		char szLen[MARCREC_SIZE_FIELDLEN];		// Длина поля
		char szOffset[MARCREC_SIZE_OFFSETLEN];	// Смещение поля от начала блока данных
	};

	// Маркер записи
	struct TMarker {
		char szLength[MARCREC_SIZE_RECLEN];	// Длина записи

		char cStatus;	/* Статус записи
						c = измененная
						d = удаленная
						n = новая
						o = предварительно составленная запись более высокого уровня
						p = предварительно составленная неполная, допубликационная запись
						*/

		char cTypeRecord;	/* Тип записи
							a = текстовые материалы, печатные
							b = текстовые материалы, рукописные
							с = музыкальные партитуры, печатные
							d = музыкальные партитуры, рукописные
							е = картографические материалы, печатные
							f = картографические материалы, рукописные
							g = проекционные и видеоматериалы (кинофильмы,
								диафильмы, слайды,пленочные материалы, видеозаписи)
							i = звукозаписи, немузыкальные
							j = звукозаписи, музыкальные
							к = двухмерная графика (иллюстрации, чертежи и т. п.)
							l = компьютерная среда (программы, базы данных и т. п.)
							m = информация на нескольких носителях (например, книга
								с приложением программ на дискете, CD и т. п.)
							r = трехмерные художественные объекты и реалии
							*/

		char cBiblioLevel;	/* Библиографический уровень
							а = аналитический - документ, является частью физической единицы
								(составная часть)
								Например: статья в журнале, продолжающаяся колонка или заметка
								внутри журнала, отдельный доклад в сборнике трудов конференции.
							m = монографический - документ, представляет собой физически единое
								целое или издается в заранее определенном количестве частей.
								Например: отдельная книга, многотомное издание в целом, том
								многотомного издания, выпуск сериального издания. 
							s = сериальный - документ, выпускаемый последовательными частями и
								рассчитанный на издание в течение неопределенного периода времени.
								(Общая часть библиографического описания сериального издания).
							с = подборка - искусственно скомплектованная библиографическая
								единица. Например: собрание брошюр в коробке или папке. 
								этот код используется только при составлении библиографического
								описания подборки.
							*/

		char cHierarchyLevel;	/* Код иерархического уровня
								# = иерархическая связь не определена
								0 = иерархическая связь отсутствует
								1 = запись высшего уровня
								2 = запись ниже высшего уровня (любая запись ниже высшего уровня)
								*/

		char cReserved1;	// Не определено (содержит #)
		char cIndLength;	// Длина индикатора (содержит 2)
		char cIdLength;		// Длина идентификатора подполя (содержит 2)
		char szBaseAddr[5];	// Базовый адрес данных относительно начала записи

		char cCodingLevel;	/* Уровень кодирования
							# = полный уровень
								Корректно составленная запись о полностью каталогизированном
								документе, подготовленная для использования в Электронном
								каталоге или для обмена. (Запись, составленная на основе
								каталогизируемого документа, у которой заполнены все поля
								и подполя со статусами "обязательное" и "условно обязательное").
							1 = подуровень 1
								Запись, составленная на основе каталожной карточки
								(ретроконверсия) или импортированная из другого формата, не
								предоставляющего достаточно данных для корректного заполнения
								всех обязательных (в т.ч. условно обязательных) элементов
								формата, и неоткорректированная по документу.
							2 = подуровень 2
								Опознавательная запись (например, допубликационная запись,
								запись на документ не проходивший каталогизацию) 
							3 = подуровень 3
								Не полностью каталогизированный документ.
								(Запись, на документ в процессе каталогизации)
							*/

		char cReferenceForm;	/* Форма каталогизационного описания
								# - запись составлена по правилам ISBD
								i - запись составлена не полностью по правилам ISBD,
									(отдельные поля соответствуют положениям ISBD)
								*/

		char cReserved2;	// Не определено (содержит #)
		char szMapPlan[4];	// План справочника (содержит 450#)
	};

	#pragma pack()

protected:
	// Список полей Marc-записи
	TFieldList FieldList;
	// Список встроенных полей Marc-записи
	TFieldList InsFieldList;
	// Дескриптор перекодировки
	iconv_t hRecodeIconv;
	// Тип записи
	int RecordType;

public:
	// Маркер Marc-записи
	TMarker Marker;
	// Флаги распаковки записей
	enum { UnpackFlagLiber = 0x00000001, UnpackFlagMarc21 = 0x00000002, UnpackFlagCheckDlm = 0x00000004};
	// Типы записей
	typedef enum { RecordTypeRusmarc = 1, RecordTypeMarc21 = 2 } TRecordType;

protected:
	// Упаковка поля Marc-записи в строку
	void PackField(TField &Field, std::string &strMarcField, bool bInsField = false);
	// Распаковка поля Marc-записи
	void UnpackField(int iFieldLabel, const char *pMarcField, int iFieldLen,
		TField &Field);
	// Конвертирование строки с использованием iconv
	bool IconvRecode(iconv_t hIconv, std::string &strString);

public:
	// Получение ссылки на конец списка полей
	inline TFieldRef NullFieldRef()
	{
		return FieldList.end();
	};
	// Получение ссылки на конец списка подполей
	inline TSubFieldRef NullSubFieldRef(const TFieldRef &FieldRef)
	{
		return FieldRef->SubFieldList.end();
	};
	// Получение ссылки на конец списка подполей
	inline TFieldRef NullInsFieldRef()
	{
		return InsFieldList.end();
	};

public:
	CMarcRec();
	CMarcRec(iconv_t hNewRecodeIconv);
	~CMarcRec();

	// Очистка Marc-записи
	void Clear();
	// Установка декскриптора перекодировки
	void SetRecodeHandle(iconv_t hNewRecodeIconv);
	// Установка типа записи
	void SetRecordType(TRecordType NewRecordType);
	// Упаковка Marc-записи в строку
	int Pack(std::string &strMarcRec);
	// Распаковка Marc-записи из строки
	int Unpack(const std::string &strMarcRec, int iFlags = 0);

	// Чтение Marc-записи из файла
	int ReadFromFile(FILE *MarcFile, int iFlags = 0);
	// Вывод Marc-записи в файл
	int WriteToFile(FILE *MarcFile);
	// Чтение Marc-записи из потока
	int ReadFromStream(std::istream &MarcStream);
	// Вывод Marc-записи в поток
	int WriteToStream(std::ostream &MarcStream);

	// Получение поля
	int GetField(int iFieldLabel, const TFieldRef &PrevFieldRef, TFieldRef &ResFieldRef);
	// Получение подполя
	int GetSubField(TField &Field, char cSubFieldLabel,
		const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef);
	inline int GetSubField(const TFieldRef &FieldRef, char cSubFieldLabel,
		const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef)
	{
		return GetSubField(*FieldRef, cSubFieldLabel, PrevSubFieldRef, ResSubFieldRef);
	}
	// Получение подполя с встроенным полем
	int GetInsSubField(TField &Field, char cSubFieldLabel, int iInsFieldLabel,
		const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef);
	inline int GetInsSubField(const TFieldRef &FieldRef, char cSubFieldLabel, int iInsFieldLabel,
		const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef)
	{
		return GetInsSubField(*FieldRef, cSubFieldLabel, iInsFieldLabel,
			PrevSubFieldRef, ResSubFieldRef);
	}
	// Получение встроенного поля
	int GetInsField(TSubField &SubField, TFieldRef &InsFieldRef);
	int GetInsField(const TSubFieldRef &SubFieldRef, TFieldRef &InsFieldRef)
	{
		return GetInsField(*SubFieldRef, InsFieldRef);
	}

	// Создание поля
	int CreateField(TFieldRef &FieldRef);
	// Создание подполя
	int CreateSubField(const TFieldRef &FieldRef, TSubFieldRef &SubFieldRef);
	// Создание встроенного поля
	int CreateInsField(const TSubFieldRef &SubFieldRef, TFieldRef &InsFieldRef);

	// Удаление поля
	int DeleteField(const TFieldRef &FieldRef);
	// Удаление подполя
	int DeleteSubField(TField &Field, const TSubFieldRef &SubFieldRef);
	// Удаление пустых полей в записи
	int DeleteEmpty(bool bRecursive = false);
	// Удаление пустых подполей в поле
	int DeleteEmpty(const TFieldRef &FieldRef);
};

#endif
