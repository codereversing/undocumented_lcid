#include <Windows.h>
#include <assert.h>
#include <stdio.h>

#define DECLARE_UNICODE_STRING(_var, _string) \
const WCHAR _var ## _buffer[] = _string; \
UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer }

typedef struct _UNICODE_STRING {
    USHORT  Length;
    USHORT  MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef NTSTATUS(__stdcall* pRtlLCIDToCultureName)(ULONG lcid, PUNICODE_STRING name);
typedef NTSTATUS(__stdcall* pRtlLcidToLocaleName)(ULONG lcid, PUNICODE_STRING name,
    DWORD reserved, BYTE encoded);
typedef NTSTATUS(__stdcall* pRtlCultureNameToLCID)(PUNICODE_STRING name, PULONG lcid);
typedef NTSTATUS(__stdcall* pRtlLocaleNameToLcid)(PWSTR name, PULONG lcid, BYTE magic);
typedef NTSTATUS(__stdcall* pRtlConvertLCIDToString)(ULONG lcid, ULONG value, ULONG precision,
    PWSTR wstr_lcid, ULONG length);

int main(int argc, char* argv[])
{
    HMODULE hmodule = LoadLibrary(L"ntdll.dll");
    pRtlLCIDToCultureName LCIDToCulture =
        (pRtlLCIDToCultureName)GetProcAddress(hmodule, "RtlLCIDToCultureName");
    pRtlLcidToLocaleName LcidToLocale =
        (pRtlLcidToLocaleName)GetProcAddress(hmodule, "RtlLcidToLocaleName");
    pRtlCultureNameToLCID CultureToLCID =
        (pRtlCultureNameToLCID)GetProcAddress(hmodule, "RtlCultureNameToLCID");
    pRtlLocaleNameToLcid LocaleToLcid =
        (pRtlLocaleNameToLcid)GetProcAddress(hmodule, "RtlLocaleNameToLcid");
    pRtlConvertLCIDToString LCIDToString =
        (pRtlConvertLCIDToString)GetProcAddress(hmodule, "RtlConvertLCIDToString");
    NTSTATUS ret = 0;
    DECLARE_UNICODE_STRING(name, L"dummystringinitializer");

    if (LCIDToCulture && LcidToLocale && CultureToLCID && LocaleToLcid && LCIDToString)
    {
        //Test RtlLCIDToCultureName and LCIDCultureNameToLCID
        ULONG lcid_enus = 0x0409;
        ULONG lcid_ruru = 0x0419;
        ret = LCIDToCulture(lcid_enus, &name);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_enus, name.Buffer);
        ret = LCIDToCulture(lcid_ruru, &name);
        wprintf(L"[%i] LCID: %i  Culture name: %s\n", ret, lcid_ruru, name.Buffer);

        ULONG ret_enus = 0x0;
        ULONG ret_ruru = 0x0;
        DECLARE_UNICODE_STRING(wstr_enus, L"en-US");
        DECLARE_UNICODE_STRING(wstr_ruru, L"ru-RU");
        ret = CultureToLCID(&wstr_enus, &ret_enus);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_enus, wstr_enus.Buffer);
        ret = CultureToLCID(&wstr_ruru, &ret_ruru);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n\n", ret, lcid_ruru, wstr_ruru.Buffer);

        assert(ret_enus == lcid_enus);
        assert(ret_ruru == lcid_ruru);
        //End RtlLCIDToCultureName and LCIDCultureNameToLCID

        //Test RtlLcidToLocaleName and RtlLocaleNameToLcid
        ULONG lcid_dedephoneb = 0x10407; //Phone book sorting flag added
        ULONG lcid_esve = 0x200A;
        ret = LcidToLocale(lcid_dedephoneb, &name, 0x00, 0x02);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_dedephoneb, name.Buffer);
        ret = LcidToLocale(lcid_esve, &name, 0x00, 0x00);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_esve, name.Buffer);

        ULONG ret_dedephoneb = 0x0;
        ULONG ret_esve = 0x0;
        DECLARE_UNICODE_STRING(wstr_uzlatnuz, L"de-DE_phoneb");
        DECLARE_UNICODE_STRING(wstr_esve, L"es-VE");
        ret = LocaleToLcid(wstr_uzlatnuz.Buffer, &ret_dedephoneb, 3);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, ret_dedephoneb, wstr_uzlatnuz.Buffer);
        ret = LocaleToLcid(wstr_esve.Buffer, &ret_esve, 3);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n\n", ret, ret_esve, wstr_esve.Buffer);

        assert(lcid_dedephoneb == ret_dedephoneb);
        assert(lcid_esve == ret_esve);
        //End RtlLcidToLocaleName and RtlLocaleNameToLcid

        //Test ConvertLCIDToString
        ULONG lcid_is = 0x040F;
        ret = LCIDToString(lcid_is, 0x10, 0x4, name.Buffer, name.MaximumLength);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_is, name.Buffer);
        ret = LCIDToString(lcid_is, 0x10, 0x8, name.Buffer, name.MaximumLength);
        wprintf(L"[%i] LCID: %i  Culture/Locale name: %s\n", ret, lcid_is, name.Buffer);
        //End ConvertLCIDToString
    }
    else
        wprintf(L"One or more functions could not be exported.\n");

    return 0;
}