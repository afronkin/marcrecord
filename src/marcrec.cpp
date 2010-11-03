/*
 * Copyright (c) Tver Regional Scientific Library
 * Author: Alexander Fronkin
 *
 * Version 1.1 (1 Jan 2003)
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "marcrec.h"

// ------------------
// Конструктор класса
// ------------------
CMarcRec::CMarcRec()
{
	// Очистка Marc-записи
	this->hRecodeIconv = (iconv_t)-1;
	Clear();
	// Установка типа записи
	SetRecordType(RecordTypeRusmarc);
}

CMarcRec::CMarcRec(iconv_t hNewRecodeIconv)
{
	// Очистка Marc-записи
	hRecodeIconv = (iconv_t)-1;
	Clear();
	// Установка типа записи
	SetRecordType(RecordTypeRusmarc);
	// Установка таблицы перекодировки
	SetRecodeHandle(hNewRecodeIconv);
}

// -----------------
// Деструктор класса
// -----------------
CMarcRec::~CMarcRec()
{
	// Очистка Marc-записи
	Clear();
}

// -------------------
// Очистка Marc-записи
// -------------------
void CMarcRec::Clear()
{
	// Очистка списков полей
	InsFieldList.clear();
	FieldList.clear();

	// Заполнение маркера
	memset(Marker.szLength, ' ', sizeof(Marker.szLength));
	Marker.cStatus = 'n';
	Marker.cTypeRecord = 'a';
	Marker.cBiblioLevel = 'm';
	Marker.cHierarchyLevel = ' ';
	Marker.cReserved1 = ' ';
	Marker.cIndLength = '2';
	Marker.cIdLength = '2';
	memset(Marker.szBaseAddr, ' ', sizeof(Marker.szBaseAddr));
	Marker.cCodingLevel = ' ';
	Marker.cReferenceForm = ' ';
	Marker.cReserved2 = ' ';
	memcpy(Marker.szMapPlan, "450 ", sizeof(Marker.szMapPlan));
}

// -------------------------------
// Установка таблицы перекодировки
// -------------------------------
void CMarcRec::SetRecodeHandle(iconv_t hNewRecodeIconv)
{
	hRecodeIconv = hNewRecodeIconv;
}

// ---------------------
// Установка типа записи
// ---------------------
void CMarcRec::SetRecordType(CMarcRec::TRecordType NewRecordType)
{
	RecordType = NewRecordType;
}

// -----------------------------
// Упаковка Marc-записи в строку
// -----------------------------
int CMarcRec::Pack(std::string &strMarcRec)
{
//	int iCmdResult;
	TFieldRef FieldRef, EndFieldRef;
	std::string strMarcField;
	size_t iFieldLen, iFieldOffset;
	int iFieldNo;
	char szCnvBuf[30];
	TMarker *pMarker;

	// Очистка Marc-записи
	strMarcRec.erase();

	// Копирование маркера
	strMarcRec.append((char *)&Marker, sizeof(TMarker));

/*	// Удаление пустых элементов в записи
	if((iCmdResult = DeleteEmpty(true)) != MARCREC_SUCCESS)
		return iCmdResult;*/

	// Упаковка списка полей
	strMarcRec.resize(sizeof(TMarker) + sizeof(TMapItem) * FieldList.size() + 1);
	iFieldOffset = 0;
	EndFieldRef = FieldList.end();
	for(iFieldNo = 0, FieldRef = FieldList.begin(); FieldRef != EndFieldRef; \
		FieldRef++, iFieldNo++)
	{
		// Упаковка поля
		strMarcField.erase();
		PackField(*FieldRef, strMarcField);
		strMarcRec.append(strMarcField);

		iFieldLen = strMarcField.size();
		sprintf((char *)strMarcRec.data() + sizeof(Marker) + sizeof(TMapItem) * iFieldNo,
			"%03d%04d%05d", FieldRef->iLabel, iFieldLen, iFieldOffset);
		iFieldOffset += iFieldLen;
	}
	strMarcRec[sizeof(Marker) + sizeof(TMapItem) * iFieldNo] = MARCREC_SYMBOL_IS2;
	strMarcRec += MARCREC_SYMBOL_IS3;

	// Заполнение атрибутов записи
	pMarker = (TMarker *)strMarcRec.data();
	sprintf(szCnvBuf, "%05d", strMarcRec.size());
	memcpy(pMarker->szLength, szCnvBuf, sizeof(pMarker->szLength));
	sprintf(szCnvBuf, "%05d", sizeof(TMarker) + sizeof(TMapItem) * FieldList.size() + 1);
	memcpy(pMarker->szBaseAddr, szCnvBuf, sizeof(pMarker->szBaseAddr));

	return MARCREC_SUCCESS;
}

// ----------------------------------
// Упаковка поля Marc-записи в строку
// ----------------------------------
void CMarcRec::PackField(TField &Field, std::string &strMarcField, bool bInsField /* = false */)
{
	TSubFieldRef SubFieldRef, EndSubFieldRef;
	TFieldRef InsFieldRef;
	char szCnvBuf[30];

	// Упаковка атрибутов поля
	if(Field.iLabel >= 10)
		strMarcField.append(Field.szInd, MARCREC_SIZE_IND);

	// Упаковка списка подполей
	EndSubFieldRef = Field.SubFieldList.end();
	for(SubFieldRef = Field.SubFieldList.begin(); SubFieldRef != EndSubFieldRef; \
		SubFieldRef++)
	{
		// Упаковка подполя
		if(Field.iLabel >= 10) {
			strMarcField += MARCREC_SYMBOL_IS1;
			strMarcField += SubFieldRef->cLabel;
		}
		if(SubFieldRef->InsFieldRef == InsFieldList.end())
			strMarcField.append(SubFieldRef->strValue);
		else {
			// Упаковка встроенного поля
			InsFieldRef = SubFieldRef->InsFieldRef;
			sprintf(szCnvBuf, "%03d", InsFieldRef->iLabel);
			strMarcField.append(szCnvBuf);
			PackField(*InsFieldRef, strMarcField, true);
		}
	}
	if(!bInsField)
		strMarcField += MARCREC_SYMBOL_IS2;
/*	if(strMarcField[strMarcField.size() - 1] != MARCREC_SYMBOL_IS2)
		strMarcField += MARCREC_SYMBOL_IS2;*/
}

// --------------------------------
// Распаковка Marc-записи из строки
// --------------------------------
int CMarcRec::Unpack(const std::string &strMarcRec, int iFlags)
{
	size_t iMarcRecSize;
	char *pMarcRec, *pMarcFields;
	size_t iBaseAddr;
	int iFieldNo;
	TMapItem *pMapItem;
	int iFieldLabel, iFieldLen, iFieldOffset;
	TField Field;
	char szCnvBuf[30];

	// Проверка аргументов
	iMarcRecSize = strMarcRec.size();
	if(iMarcRecSize < sizeof(TMarker))
		return MARCREC_ERROR_BADRECORD;

	// Определение типа записи
	if(iFlags & UnpackFlagMarc21)
		SetRecordType(RecordTypeMarc21);

	// Очистка списка полей
	Clear();

	try {
		// Копирование маркера
		pMarcRec = (char *)strMarcRec.data();
		memcpy(&Marker, pMarcRec, sizeof(TMarker));

		// Определение адреса начала данных
		memcpy(szCnvBuf, Marker.szBaseAddr, sizeof(Marker.szBaseAddr));
		szCnvBuf[sizeof(Marker.szBaseAddr)] = '\0';
		iBaseAddr = atol(szCnvBuf);
//		if(sscanf(Marker.szBaseAddr, "%5d", &iBaseAddr) != 1)
//			throw MARCREC_ERROR_BADRECORD;
		pMarcFields = pMarcRec + iBaseAddr;
		if(pMarcFields > pMarcRec + iMarcRecSize)
			throw MARCREC_ERROR_BADRECORD;

		// Распаковка полей
		pMapItem = (TMapItem *)(pMarcRec + sizeof(TMarker));
		for(iFieldNo = 0; (char *)pMapItem < pMarcFields - 1; iFieldNo++, pMapItem++) {
			// Получение метки поля
			memcpy(szCnvBuf, pMapItem->szLabel, MARCREC_SIZE_LABEL);
			szCnvBuf[MARCREC_SIZE_LABEL] = '\0';
			iFieldLabel = atol(szCnvBuf);
			// Получение длины поля
			memcpy(szCnvBuf, pMapItem->szLen, MARCREC_SIZE_FIELDLEN);
			szCnvBuf[MARCREC_SIZE_FIELDLEN] = '\0';
			iFieldLen = atol(szCnvBuf);
			// Получение смещения поля
			memcpy(szCnvBuf, pMapItem->szOffset, MARCREC_SIZE_OFFSETLEN);
			szCnvBuf[MARCREC_SIZE_OFFSETLEN] = '\0';
			iFieldOffset = atol(szCnvBuf);
			// Проверка на запись из Liber
			if(iFlags & UnpackFlagLiber) {
				// Корректировка смещения
				iFieldOffset -= (int)iBaseAddr;
			}
			// Проверка структуры записи
			if(iBaseAddr + iFieldOffset + iFieldLen > iMarcRecSize)
				throw MARCREC_ERROR_BADRECORD;

			// Распаковка данных поля
			UnpackField(iFieldLabel, pMarcFields + iFieldOffset, iFieldLen, Field);

			// Занесение поля в список полей
			FieldList.push_back(Field);
		}
	} catch(int iErrCode) {
		Clear();
		return iErrCode;
	}

	return MARCREC_SUCCESS;
}

// ---------------------------
// Распаковка поля Marc-записи
// ---------------------------
void CMarcRec::UnpackField(int iFieldLabel, const char *pMarcField, int iFieldLen,
	TField &Field)
{
	TSubField SubField;
	TField InsField;
	TFieldRef InsFieldRef;
	const char *pInsMarcField;

	// Заполнение атрибутов поля
	Field.iLabel = iFieldLabel;
	Field.szInd[0] = Field.szInd[1] = '\0';
	if(iFieldLabel >= 10) {
		if(*pMarcField != MARCREC_SYMBOL_IS1) {
			Field.szInd[0] = *(char *)pMarcField++;
			iFieldLen--;
		}
		if(*pMarcField != MARCREC_SYMBOL_IS1) {
			Field.szInd[1] = *(char *)pMarcField++;
			iFieldLen--;
		}
	}

	// Заполнение списка подполей
	Field.SubFieldList.clear();
	SubField.cLabel = '\0';
	SubField.strValue.erase();
	SubField.InsFieldRef = InsFieldList.end();
	for(iFieldLen--; iFieldLen > 0; pMarcField++, iFieldLen--) {
		if(iFieldLen == 1)
			iFieldLen = 1;
		if(*pMarcField == MARCREC_SYMBOL_IS1) {
			if(SubField.strValue != "" || SubField.InsFieldRef != InsFieldList.end()) {
				if(hRecodeIconv != (iconv_t)-1)
					IconvRecode(hRecodeIconv, SubField.strValue);
				Field.SubFieldList.push_back(SubField);
			}
			SubField.cLabel = *++pMarcField;
			SubField.strValue.erase();
			SubField.InsFieldRef = InsFieldList.end();
			iFieldLen--;
			// Распаковка встроенного поля
			if(RecordType == RecordTypeRusmarc && SubField.cLabel == '1') {
				pInsMarcField = pMarcField + 1;
				for(pMarcField = pInsMarcField; iFieldLen > 0; pMarcField++, iFieldLen--) {
					if(*pMarcField == MARCREC_SYMBOL_IS2)
						break;
					if(*pMarcField == MARCREC_SYMBOL_IS1 && iFieldLen > 1 &&
						*(pMarcField + 1) == '1')
						break;
				}
				UnpackField(atol(std::string(pInsMarcField, 3).c_str()), pInsMarcField + 3,
					(int)(pMarcField - pInsMarcField) - 2, InsField);
				pMarcField--;
				iFieldLen++;

				// Добавление встроенного поля в список
				InsFieldRef = InsFieldList.insert(InsFieldList.end(), InsField);
				SubField.InsFieldRef = InsFieldRef;
			}
		} else
			SubField.strValue += *pMarcField;
	}
	if(SubField.strValue != "" || SubField.InsFieldRef != InsFieldList.end()) {
		if(hRecodeIconv != (iconv_t)-1)
			IconvRecode(hRecodeIconv, SubField.strValue);
		Field.SubFieldList.push_back(SubField);
	}
}

// ---------------------------
// Чтение Marc-записи из файла
// ---------------------------
int CMarcRec::ReadFromFile(FILE *MarcFile, int iFlags)
{
	int iCmdResult;
	std::string strMarcRec;
	size_t iMarcRecSize;
	int iSymbol;
	char szMarcRecSize[MARCREC_SIZE_RECLEN + 1], *pBuf;

	// Проверка аргументов
	if(MarcFile == NULL)
		return MARCREC_ERROR_WRONGARGS;

	// Пропуск нецифровых символов
	while(!feof(MarcFile)) {
		iSymbol = fgetc(MarcFile);
		if(iSymbol >= '0' && iSymbol <= '9')
			break;
	}
	if(feof(MarcFile))
		return MARCREC_ERROR_EOF;
	szMarcRecSize[0] = (char)iSymbol;

	// Чтение Marc-записи из файла
	try {
		if(fread((void *)(szMarcRecSize + 1), 1, MARCREC_SIZE_RECLEN - 1, MarcFile) !=
			MARCREC_SIZE_RECLEN - 1)
			return (feof(MarcFile) ? MARCREC_ERROR_EOF : MARCREC_ERROR_FILE);

		szMarcRecSize[MARCREC_SIZE_RECLEN] = '\0';
		iMarcRecSize = atol(szMarcRecSize);
		if(iMarcRecSize < MARCREC_SIZE_RECLEN)
			throw MARCREC_ERROR_FILE;
		
		pBuf = (char *)malloc(MARCREC_SIZE_RECLEN + iMarcRecSize);
		memcpy(pBuf, (void *)szMarcRecSize, MARCREC_SIZE_RECLEN);
		if(fread(pBuf + MARCREC_SIZE_RECLEN, 1,
			iMarcRecSize - MARCREC_SIZE_RECLEN, MarcFile) != iMarcRecSize - MARCREC_SIZE_RECLEN)
		{
			free(pBuf);
			throw MARCREC_ERROR_FILE;
		}

//		strMarcRec.assign(pBuf, MARCREC_SIZE_RECLEN + iMarcRecSize);
		strMarcRec.assign(pBuf, iMarcRecSize);
		free(pBuf);
		throw MARCREC_SUCCESS;
	} catch(int iErrCode) {
		// Проверка разделителя записей
		if((iFlags & UnpackFlagCheckDlm) && strMarcRec[strMarcRec.length() - 1] != MARCREC_SYMBOL_IS3) {
			// Пропуск всех символов файла записей до разделителя
			while(!feof(MarcFile)) {
				iSymbol = fgetc(MarcFile);
				if(iSymbol == MARCREC_SYMBOL_IS3)
					break;
			}

			return (iErrCode == MARCREC_SUCCESS ? MARCREC_ERROR_BADRECORD : iErrCode);
		}

		if(iErrCode != MARCREC_SUCCESS)
			return iErrCode;
	}

	// Распаковка Marc-записи
	if((iCmdResult = Unpack(strMarcRec, iFlags)) != MARCREC_SUCCESS)
		return iCmdResult;

	return MARCREC_SUCCESS;
}

// ------------------------
// Вывод Marc-записи в файл
// ------------------------
int CMarcRec::WriteToFile(FILE *MarcFile)
{
	int iCmdResult;
	std::string strMarcRec;
	size_t iMarcRecSize;

	// Проверка аргументов
	if(MarcFile == NULL)
		return MARCREC_ERROR_WRONGARGS;

	// Упаковка Marc-записи
	if((iCmdResult = Pack(strMarcRec)) != MARCREC_SUCCESS)
		return iCmdResult;

	// Вывод Marc-записи в файл
	iMarcRecSize = strMarcRec.size();
	if(fwrite(strMarcRec.data(), 1, iMarcRecSize, MarcFile) != iMarcRecSize)
		return MARCREC_ERROR_FILE;

	return MARCREC_SUCCESS;
}

// ----------------------------
// Чтение Marc-записи из потока
// ----------------------------
int CMarcRec::ReadFromStream(std::istream &MarcStream)
{
	return MARCREC_SUCCESS;
}

// ----------------------------
// Вывод Marc-записи в поток
// ----------------------------
int CMarcRec::WriteToStream(std::ostream &MarcStream)
{
	return MARCREC_SUCCESS;
}

// --------------
// Получение поля
// --------------
int CMarcRec::GetField(int iFieldLabel, const TFieldRef &PrevFieldRef, TFieldRef &ResFieldRef)
{
	TFieldRef FieldRef, EndFieldRef;

	// Проверка аргументов
	if(iFieldLabel < 0)
		return MARCREC_ERROR_WRONGARGS;

	// Определение начального поля для поиска
	if(PrevFieldRef != FieldList.end()) {
		FieldRef = PrevFieldRef;
		FieldRef++;
	} else
		FieldRef = FieldList.begin();
	// Поиск заданного поля
	for(EndFieldRef = FieldList.end(); FieldRef != EndFieldRef; FieldRef++) {
		if(iFieldLabel == 0 || FieldRef->iLabel == iFieldLabel) {
			ResFieldRef = FieldRef;
			return MARCREC_SUCCESS;
		}
	}

	return MARCREC_ERROR_NOTFOUND;
}

// -----------------
// Получение подполя
// -----------------
int CMarcRec::GetSubField(TField &Field, char cSubFieldLabel,
	const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef)
{
	TSubFieldRef SubFieldRef, EndSubFieldRef;

	// Определение начального подполя для поиска
	if(PrevSubFieldRef != Field.SubFieldList.end()) {
		SubFieldRef = PrevSubFieldRef;
		SubFieldRef++;
	} else
		SubFieldRef = Field.SubFieldList.begin();
	// Поиск заданного подполя
	for(EndSubFieldRef = Field.SubFieldList.end(); SubFieldRef != EndSubFieldRef; \
		SubFieldRef++)
	{
		if(cSubFieldLabel == '\0' || SubFieldRef->cLabel == cSubFieldLabel) {
			ResSubFieldRef = SubFieldRef;
			return MARCREC_SUCCESS;
		}
	}

	return MARCREC_ERROR_NOTFOUND;
}

// ------------------------------------
// Получение подполя с встроенным полем
// ------------------------------------
int CMarcRec::GetInsSubField(TField &Field, char cSubFieldLabel, int iInsFieldLabel,
	const TSubFieldRef &PrevSubFieldRef, TSubFieldRef &ResSubFieldRef)
{
	TSubFieldRef SubFieldRef, EndSubFieldRef;
	TFieldRef InsFieldRef;

	// Определение начального подполя для поиска
	if(PrevSubFieldRef != Field.SubFieldList.end()) {
		SubFieldRef = PrevSubFieldRef;
		SubFieldRef++;
	} else
		SubFieldRef = Field.SubFieldList.begin();
	// Поиск заданного подполя
	for(EndSubFieldRef = Field.SubFieldList.end(); SubFieldRef != EndSubFieldRef; \
		SubFieldRef++)
	{
		if((cSubFieldLabel == '\0' || SubFieldRef->cLabel == cSubFieldLabel) &&
			(InsFieldRef = SubFieldRef->InsFieldRef) != InsFieldList.end() &&
			InsFieldRef->iLabel == iInsFieldLabel)
		{
			ResSubFieldRef = SubFieldRef;
			return MARCREC_SUCCESS;
		}
	}

	return MARCREC_ERROR_NOTFOUND;
}

// --------------------------
// Получение встроенного поля
// --------------------------
int CMarcRec::GetInsField(TSubField &SubField, TFieldRef &InsFieldRef)
{
	TField Field;

	// Получение встроенного поля
	if(SubField.InsFieldRef != InsFieldList.end())
		InsFieldRef = SubField.InsFieldRef;
	else
		return MARCREC_ERROR_NOTFOUND;

	return MARCREC_SUCCESS;
}

// -------------
// Создание поля
// -------------
int CMarcRec::CreateField(TFieldRef &FieldRef)
{
	TField NewField;

	NewField.iLabel = 0;
	NewField.szInd[0] = NewField.szInd[1] = '\0';
	FieldRef = FieldList.insert(FieldList.end(), NewField);

	return MARCREC_SUCCESS;
}

// ----------------
// Создание подполя
// ----------------
int CMarcRec::CreateSubField(const TFieldRef &FieldRef, TSubFieldRef &SubFieldRef)
{
	TSubField NewSubField;

	NewSubField.cLabel = '\0';
	NewSubField.strValue = "";
	NewSubField.InsFieldRef = InsFieldList.end();
	SubFieldRef = FieldRef->SubFieldList.insert(FieldRef->SubFieldList.end(), NewSubField);

	return MARCREC_SUCCESS;
}

// -------------------------
// Создание встроенного поля
// -------------------------
int CMarcRec::CreateInsField(const TSubFieldRef &SubFieldRef, TFieldRef &InsFieldRef)
{
	TField NewInsField;

	NewInsField.iLabel = 0;
	NewInsField.szInd[0] = NewInsField.szInd[1] = '\0';
	InsFieldRef = InsFieldList.insert(InsFieldList.end(), NewInsField);
	SubFieldRef->InsFieldRef = InsFieldRef;

	return MARCREC_SUCCESS;
}

// -------------
// Удаление поля
// -------------
int CMarcRec::DeleteField(const TFieldRef &FieldRef)
{
	FieldList.erase(FieldRef);
	return MARCREC_SUCCESS;
}

// ----------------
// Удаление подполя
// ----------------
int CMarcRec::DeleteSubField(TField &Field, const TSubFieldRef &SubFieldRef)
{
	Field.SubFieldList.erase(SubFieldRef);
	return MARCREC_SUCCESS;
}

// ------------------------------
// Удаление пустых полей в записи
// ------------------------------
int CMarcRec::DeleteEmpty(bool bRecursive /* = false */)
{
	TFieldRef FieldRef, EndFieldRef, EmptyFieldRef = FieldList.end();
	int iCmdResult;

	// Перебор полей
	FieldRef = FieldList.begin();
	EndFieldRef = FieldList.end();
	while(FieldRef != EndFieldRef) {
		if(bRecursive) {
			// Удаление пустых подполей
			iCmdResult = DeleteEmpty(FieldRef);
			if(iCmdResult != MARCREC_SUCCESS)
				return iCmdResult;
		}

		if(FieldRef->SubFieldList.size() == 0)
			EmptyFieldRef = FieldRef;

		FieldRef++;

		if(EmptyFieldRef != FieldList.end()) {
			FieldList.erase(EmptyFieldRef);
			EmptyFieldRef = FieldList.end();
		}
	}

	return MARCREC_SUCCESS;
}

// -------------------------------
// Удаление пустых подполей в поле
// -------------------------------
int CMarcRec::DeleteEmpty(const TFieldRef &FieldRef)
{
	TSubFieldRef SubFieldRef, EndSubFieldRef, EmptySubFieldRef;
	TSubFieldRef InsSubFieldRef, EndInsSubFieldRef, EmptyInsSubFieldRef;
	TFieldRef InsFieldRef;
	int iCmdResult;

	// Проверка аргументов
	if(FieldRef == FieldList.end())
		return MARCREC_ERROR_WRONGARGS;

	// Перебор подполей
	SubFieldRef = FieldRef->SubFieldList.begin();
	EndSubFieldRef = FieldRef->SubFieldList.end();
	EmptySubFieldRef = EndSubFieldRef;
	while(SubFieldRef != EndSubFieldRef) {
		if(SubFieldRef->InsFieldRef != InsFieldList.end()) {
			// Обработка встроенного поля
			InsFieldRef = SubFieldRef->InsFieldRef;

			// Перебор подполей встроенного поля
			InsSubFieldRef = InsFieldRef->SubFieldList.begin();
			EndInsSubFieldRef = InsFieldRef->SubFieldList.end();
			EmptyInsSubFieldRef = EndInsSubFieldRef;
			while(InsSubFieldRef != EndInsSubFieldRef) {
				if(InsSubFieldRef->strValue == "")
					EmptyInsSubFieldRef = InsSubFieldRef;

				InsSubFieldRef++;

				// Удаление пустого подполя встроенного поля
				if(EmptyInsSubFieldRef != EndInsSubFieldRef) {
					InsFieldRef->SubFieldList.erase(EmptyInsSubFieldRef);
					EmptyInsSubFieldRef = EndInsSubFieldRef;
				}
			}

			// Проверка числа оставшихся элементов во встроенном поле
			if(InsFieldRef->SubFieldList.size() == 0)
				EmptySubFieldRef = SubFieldRef;
		} else if(SubFieldRef->strValue == "")
			EmptySubFieldRef = SubFieldRef;

		SubFieldRef++;

		// Удаление пустого подполя
		if(EmptySubFieldRef != FieldRef->SubFieldList.end()) {
			FieldRef->SubFieldList.erase(EmptySubFieldRef);
			EmptySubFieldRef = EndSubFieldRef;
		}
	}

	return MARCREC_SUCCESS;
}

// ---------------------------------------------
// Конвертирование строки с использованием iconv
// ---------------------------------------------
bool CMarcRec::IconvRecode(iconv_t hIconv, std::string &strString)
{
	size_t iSrcLen = strString.size(), iDestLen, iNonRevConverted;
	char *pSrc = (char *)strString.data(), *pDest, *pOutBuf;
	int iErrCode;

	// Проверка аргументов
	if(hIconv == (iconv_t)-1)
		return false;

	// Создание буфера
	pOutBuf = pDest = (char *)malloc(iDestLen = iSrcLen * 2);
	if(pOutBuf == NULL)
		return false;

	// Конвертирование
	iconv(hIconv, NULL, NULL, NULL, NULL);
	iErrCode = 0;
	while(iSrcLen > 0 && iDestLen > 0) {
		iNonRevConverted = iconv(hIconv, &pSrc, &iSrcLen, &pDest, &iDestLen);
		if(iNonRevConverted == (size_t)-1) {
			iErrCode = errno;
			if(iErrCode == EILSEQ) {
				pSrc++;
				iSrcLen--;
				continue;
			}
		}
		break;
	}

	// Копирование строки в результат
	if(iErrCode == 0 || iErrCode == EILSEQ || iErrCode == EINVAL) {
		strString.assign(pOutBuf, pDest - pOutBuf);
		free(pOutBuf);
		return true;
	}

	free(pOutBuf);
	return false;
}
