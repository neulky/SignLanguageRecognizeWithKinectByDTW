#include"Speech.h"

int ResultSpeech(string recognizeResult)
{
	ISpVoice *pSpVoice;        // ��ҪCOM�ӿ�
	CoInitialize(NULL);         // COM��ʼ��
	// ��ȡISpVoice�ӿ�
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pSpVoice);
	pSpVoice->SetVolume(80);

	const char* ch = recognizeResult.data();

	WCHAR wszClassName[256];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch) + 1, wszClassName,
		sizeof(wszClassName) / sizeof(wszClassName[0]));

	pSpVoice->Speak(wszClassName, SPF_DEFAULT, NULL);
	pSpVoice->Release();
	::CoUninitialize();
	return 0;
}