#include <SDKDDKVer.h>

#include <tchar.h>
#include <string>
#include <iostream>

#include "Hap.h"

class MyAccessoryInformation : public Hap::AccessoryInformation
{
public:
	MyAccessoryInformation()
	{
		_identify.onWrite([this](Hap::Obj::wr_prm& p, Hap::Characteristic::Identify::V v) -> void {
			Log("MyAccessoryInformation: write Identify\n");
		});
	}
};

class MyLb : public Hap::Lightbulb
{
public:
	MyLb()
	{
		On().onRead([this](Hap::Obj::rd_prm& p) -> void {
			Log("MyLb: read On: %d\n", On().Value());
		});

		On().onWrite([this](Hap::Obj::wr_prm& p, Hap::Characteristic::On::V v) -> void {
			Log("MyLb: write On: %d -> %d\n", On().Value(), v);
		});
	}
};

class MyAcc : public Hap::Accessory<2>
{
private:
	MyAccessoryInformation ai;
	MyLb lb;
public:
	MyAcc() : Hap::Accessory<2>()
	{
		AddService(&ai);
		AddService(&lb);
	}
};

class MyDb : public Hap::DbStatic<1>
{
private:
	MyAcc acc;
public:
	MyDb()
	{
		AddAcc(&acc);
	}

	// db initialization:
	//	set aids
	void Init(Hap::iid_t aid)
	{
		acc.init(aid);
		auto a = GetAcc(1);
		auto b = GetAcc(2);

		printf("acc %p  a %p  b %p\n", &acc, a, b);
	}
};

// static/global data of this accessory server
//	data must be initialized on first time start or on reset
//	some data must be stored in non-volatile storage and restored upon startup
Hap::Config Hap::config;
MyDb db;					// accessory attribute database
Hap::Pairings pairings;		// pairing records 
Hap::Crypt::Ed25519 keys;	// crypto keys			

// main HTTP server
Hap::Http::Server http(db, pairings, keys);

int main()
{
	Hap::config.port = swap_16(7889);

	//sha2_test();
	//srp_test();
	//return 0;
	
	// create and start servers
	Hap::Mdns* mdns = Hap::Mdns::Create();
	Hap::Tcp* tcp = Hap::Tcp::Create(&http);

	// Init global data	TODO: save/restore to/from storage
	Hap::config.name = "esp32test";				// const char* name;	// Accessory name - used as initial Bonjour name and as	Accessory Information Service name of aid=1
	Hap::config.model = "TestModel";			// const char* model;	// Model name (Bonjour and AIS)
	Hap::config.id = "00:11:22:33:44:55";		// const char* id;		// Device ID (XX:XX:XX:XX:XX:XX, generated new on factory reset)
	Hap::config.cn = 1;							// uint32_t cn;			// Current configuration number, incremented on db change
	Hap::config.ci = 5;							// uint8_t ci;			// category identifier
	Hap::config.sf = 0							// uint8_t sf;			// status flags
		| Hap::Bonjour::NotPaired
		| Hap::Bonjour::NotConfiguredForWiFi;
	Hap::config.setup = "000-11-000";			// const char* setup	// Setup code
	Hap::config.port = swap_16(7889);			// uint16_t port;		// TCP port of HAP service
	Hap::config.BCT = 0;

	Hap::config.Update = [mdns]() -> void {
		mdns->Update();
	};

	db.Init(1);
	pairings.Init();
	keys.Init();

	mdns->Start();
	tcp->Start();

#if 0
//	Hap::sid_t sid = srv.Open();

	char str[256];
	int l;

	l = db.getDb(sid, str, sizeof(str) - 1);
	str[l] = 0;
	printf("sizeof(srv)=%d  db '%s'\n",
		sizeof(db), str);

	static const char wr[] = "{\"characteristics\":[{\"aid\":1,\"iid\":2,\"value\":true,\"ev\":true},{\"aid\":3,\"iid\":8,\"ev\":true}]}";
//	const char wr[] = "{\"characteristics\":[{\"aid\":1,\"iid\":8,\"value\":true}]}";
	static char rsp[256];
	int rsp_size = sizeof(rsp);

	auto rc = db.Write(sid, wr, sizeof(wr)-1, rsp, rsp_size);
	Log("Write: %s  rsp '%.*s'\n", Hap::Http::StatusStr(rc), rsp_size, rsp);

	static const char rd[] = "id=1.2,3.1&ev=1&meta=1&perms=1&type=1";
	rsp_size = sizeof(rsp);
	rc = db.Read(sid, rd, sizeof(rd)-1, rsp, rsp_size);
	Log("Read: %s  rsp '%.*s'\n", Hap::Http::StatusStr(rc), rsp_size, rsp);

	rsp_size = sizeof(rsp);
	rc = db.getEvents(sid, rsp, rsp_size);
	Log("Events: %s  rsp %d '%.*s'\n", Hap::Http::StatusStr(rc), rsp_size, rsp_size, rsp);

	rsp_size = sizeof(rsp);
	rc = db.getEvents(sid, rsp, rsp_size);
	Log("Events: %s  rsp %d '%.*s'\n", Hap::Http::StatusStr(rc), rsp_size, rsp_size, rsp);

	tcpServer();
#else
	char c;
	std::cin >> c;
#endif

	tcp->Stop();
	mdns->Stop();

#if 0
	srv.Process(sid, nullptr,
	[](Hap::sid_t sid, void* ctx, uint8_t* buf, uint16_t& size) -> bool {

		static bool first = true;
		static const char req[] = "";

		if (first)
		{
			size = sizeof(req);
			memcpy(buf, req, size);
			first = false;
			return true;
		}
		return false;
	},
	[](Hap::sid_t sid, void* ctx, uint8_t* buf, uint16_t len, bool close) -> bool {
		return true;
	});

	srv.Close(sid);
#endif


	return 0;
}

#include <time.h>
#include <stdlib.h>

extern "C" {
	void t_stronginitrand()
	{
		srand((unsigned)time(NULL));
	}

	void t_random(unsigned char* data, unsigned size)
	{

		for (unsigned i = 0; i < size; i++)
		{
			*data++ = /*rand()*/ i & 0xFF;
		}
	}
}

#if 0

Hap::Property::Type ty("asdfgh");
Hap::Property::InstanceId iid;
Hap::Property::EventNotifications en(false);
Hap::Property::LinkedServices<5> ls;
Hap::Property::MinValue<Hap::FormatId::Int> mvi(10);
Hap::Property::MinValue<Hap::FormatId::Float> mvf(1.1);

//	printf("sizeof(c1)=%d  type '%s'  iid %lld\n",
//		sizeof(c1), c1.Type(), c1.Iid());

//	printf("sizeof(c2)=%d  type '%s'  iid %lld\n",
//		sizeof(c2), c2.Type(), c2.Iid());

//	Hap::Characteristic::On on(3);
//	on.on();
//	int l = on.getDb(str, sizeof(str) - 1);
//	str[l] = 0;

//	printf("sizeof(on)=%d  type '%s'  iid %lld  val %d  db '%s'\n",
//		sizeof(on), on.Type(), on.Iid(), on.Value(), str);

//	lb.On().on();

//	l = lb.getDb(str, sizeof(str) - 1);
//	str[l] = 0;
//	printf("sizeof(lb)=%d  type '%s'  iid %lld  db '%s'\n",
//		sizeof(lb), lb.Type().get(), lb.Iid().get(), str);

//	Hap::Accessory<Hap::Property::Type, Hap::Property::InstanceId> acc(ty, iid);

//	printf("ty=%s  sizeof(ty)=%d FormatId %d  Size %d  Length %d  Format '%s'  Key '%s'\n",
//		ty.get(), sizeof(ty), ty.formatId(), ty.size(), ty.length(), ty.format(), ty.key());

//	printf("en=%d  sizeof(en)=%d FormatId %d  Size %d  Length %d  Format '%s'  Key '%s'\n",
//		en.get(), sizeof(en), en.formatId(), en.size(), en.length(), en.format(), en.key());
en.set(true);
printf("en=%d\n", en.get());

Hap::Property::Permissions pm(Hap::Property::Permissions::PairedRead | Hap::Property::Permissions::PairedWrite);
printf("isPairedWrite: %d  isEvent: %d\n",
	pm.isEnabled(Hap::Property::Permissions::PairedWrite),
	pm.isEnabled(Hap::Property::Permissions::Events)
);

//	ls[1] = 1234;
ls.set(2, 3456);
//	printf("sizeof(ls)=%d FormatId %d  Size %d  Length %d  ls[0] %lld  ls[1] %lld  ls[2] %lld\n",
//		sizeof(ls), ls.formatId(), ls.size(), ls.length(), ls[0], ls[1], ls.get(2));

printf("mvi %d  mvf %g\n", mvi.get(), mvf.get());

#endif

