#include <SDKDDKVer.h>

#include <tchar.h>
#include <string>

#include "Hap.h"

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

class MyAcc : public Hap::Accessory<1>
{
private:
	MyLb lb;
public:
	MyAcc() : Hap::Accessory<1>()
	{
		AddService(&lb);
	}
};

class MyDb : public Hap::Db<1>
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
	void init(Hap::iid_t aid)
	{
		acc.init(aid);
		auto a = GetAcc(1);
		auto b = GetAcc(2);

		printf("acc %p  a %p  b %p\n", &acc, a, b);
	}
} db;

int main()
{
	char str[256];
	int l;

	db.init(1);

	l = db.getDb(str, sizeof(str) - 1);
	str[l] = 0;
	printf("sizeof(srv)=%d  db '%s'\n",
		sizeof(db), str);

	const char wr[] = "{\"characteristics\":[{\"aid\":1,\"iid\":2,\"value\":true,\"ev\":true},{\"aid\":3,\"iid\":8,\"ev\":true}]}";
//	const char wr[] = "{\"characteristics\":[{\"aid\":1,\"iid\":8,\"value\":true}]}";
	char rsp[256];
	int rsp_size = sizeof(rsp);

	auto rc = db.Write(wr, sizeof(wr)-1, rsp, rsp_size);
	Log("Write: %s  rsp '%.*s'\n", Hap::HttpStatusStr(rc), rsp_size, rsp);

	const char rd[] = "id=1.2,3.1&ev=1&meta=1&perms=1&type=1";
	rsp_size = sizeof(rsp);
	rc = db.Read(rd, sizeof(rd)-1, rsp, rsp_size);
	Log("Read: %s  rsp '%.*s'\n", Hap::HttpStatusStr(rc), rsp_size, rsp);

	rsp_size = sizeof(rsp);
	rc = db.getEvents(rsp, rsp_size);
	Log("Events: %s  rsp %d '%.*s'\n", Hap::HttpStatusStr(rc), rsp_size, rsp_size, rsp);

	rsp_size = sizeof(rsp);
	rc = db.getEvents(rsp, rsp_size);
	Log("Events: %s  rsp %d '%.*s'\n", Hap::HttpStatusStr(rc), rsp_size, rsp_size, rsp);

	return 0;
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