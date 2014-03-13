
//gcc -o sdphelper sdp_helper.c -lbluetooth -Wall

#include <errno.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define	PSMHIDCTL	0x11
#define	PSMHIDINT	0x13

//ATTR 0x0001
static void my_sdp_set_ServiceClassIDList(sdp_record_t *record) {
	uuid_t hidsvcls_uuid;
	sdp_list_t *svclass_id;
	sdp_uuid16_create(&hidsvcls_uuid, HID_SVCLASS_ID);
	svclass_id = sdp_list_append(0, &hidsvcls_uuid);
	sdp_set_service_classes(record, svclass_id);
}

//ATTR 0x0004
static void my_sdp_set_ProtocolDescriptorList(sdp_record_t *record) {
	uuid_t hidp_uuid, l2cap_uuid;
	sdp_list_t *proto_l2cap, *proto_hidp, *apseq, *aproto;
	sdp_data_t *psm;
	unsigned short ctrl = PSMHIDCTL;

	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	proto_l2cap = sdp_list_append(0, &l2cap_uuid);
	psm = sdp_data_alloc(SDP_UINT16, &ctrl);
	proto_l2cap = sdp_list_append(proto_l2cap, psm);

	sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
	proto_hidp = sdp_list_append(0, &hidp_uuid);

	apseq = sdp_list_append(0, proto_l2cap);
	apseq = sdp_list_append(apseq, proto_hidp);
	aproto = sdp_list_append(0, apseq);
	sdp_set_access_protos(record, aproto);
}

//ATTR 0x0005
static void my_sdp_set_BrowseGroupList(sdp_record_t *record) {
	uuid_t browsegroup_uuid;
	sdp_list_t *browsegroup_seq;
	sdp_uuid16_create(&browsegroup_uuid, PUBLIC_BROWSE_GROUP);
	browsegroup_seq = sdp_list_append(0, &browsegroup_uuid);
	sdp_set_browse_groups(record, browsegroup_seq);
}

//ATTR 0x0006
static void my_sdp_set_LanguageBaseAttributeIDList(sdp_record_t *record) {
	sdp_lang_attr_t base_lang;
	sdp_list_t *langs = 0;
	base_lang.code_ISO639 = (0x65 << 8) | 0x6e;
	base_lang.encoding = 106;
	base_lang.base_offset = SDP_PRIMARY_LANG_BASE;
	langs = sdp_list_append(0, &base_lang);
	sdp_set_lang_attr(record, langs);
	sdp_list_free(langs, 0);
}

//ATTR 0x0009
static void my_sdp_set_BluetoothProfileDescriptorList(sdp_record_t *record) {
	sdp_profile_desc_t profile;
	sdp_list_t *pfseq;
	sdp_uuid16_create(&profile.uuid, HID_PROFILE_ID);
	profile.version = 0x0100;
	pfseq = sdp_list_append(0, &profile);
	sdp_set_profile_descs(record, pfseq);
}

//ATTR 0x000d
static void my_sdp_set_AdditionalProtocolDescriptorLists(sdp_record_t *record) {
	uuid_t hidp_uuid, l2cap_uuid;
	sdp_list_t *proto_l2cap, *proto_hidp, *apseq, *aproto;
	sdp_data_t *psm;
	unsigned short intr = PSMHIDINT;

	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	proto_l2cap = sdp_list_append(0, &l2cap_uuid);
	psm = sdp_data_alloc(SDP_UINT16, &intr);
	proto_l2cap = sdp_list_append(proto_l2cap, psm);

	sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
	proto_hidp = sdp_list_append(0, &hidp_uuid);

	apseq = sdp_list_append(0, proto_l2cap);
	apseq = sdp_list_append(apseq, proto_hidp);
	aproto = sdp_list_append(0, apseq);
	sdp_set_add_access_protos(record, aproto);
}

//ATTR 0x0100
static void my_sdp_set_ServiceName(sdp_record_t *record, const char *name) {
	sdp_attr_add_new(record, SDP_ATTR_SVCNAME_PRIMARY, SDP_TEXT_STR8, name);
}

//ATTR 0x0101
static void my_sdp_set_ServiceDescription(sdp_record_t *record, const char *desc) {
	sdp_attr_add_new(record, SDP_ATTR_SVCDESC_PRIMARY, SDP_TEXT_STR8, desc);
}

//ATTR 0x0102
static void my_sdp_set_ProviderName(sdp_record_t *record, const char *prov) {
	sdp_attr_add_new(record, SDP_ATTR_PROVNAME_PRIMARY, SDP_TEXT_STR8, prov);
}

//ATTR 0x0200
static void my_sdp_set_HID_DeviceReleaseNumber(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_DEVICE_RELEASE_NUMBER, SDP_UINT16, &val);
}

//ATTR 0x0201
static void my_sdp_set_HID_ParserVersion(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_PARSER_VERSION, SDP_UINT16, &val);
}

//ATTR 0x0202
static void my_sdp_set_HID_DeviceSubClass(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_DEVICE_SUBCLASS, SDP_UINT8, &val);
}

//ATTR 0x0203
static void my_sdp_set_HID_CountryCode(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_COUNTRY_CODE, SDP_UINT8, &val);
}

//ATTR 0x0204
static void my_sdp_set_HID_VirtualCable(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_VIRTUAL_CABLE, SDP_BOOL, &val);
}

//ATTR 0x0205
static void my_sdp_set_HID_ReconnectInitiate(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_RECONNECT_INITIATE, SDP_BOOL, &val);
}

/*
data - hid report descriptor data string.
 len - data length in byte.
*/
//ATTR 0x0206
static void my_sdp_set_HID_DescriptorList(sdp_record_t *record, void *data, int len) {
	void *dtds[2];
	void *values[2];
	unsigned char dtd2=SDP_UINT8;
	unsigned char hid_spec_type=0x22;
	unsigned char dtd_data=SDP_TEXT_STR8;
	int leng[2];
	sdp_data_t *hid_spec_lst;
	sdp_data_t *hid_spec_lst2;

	dtds[0] = &dtd2;
	values[0] = &hid_spec_type;
	dtds[1] = &dtd_data;
	values[1] = data;
	leng[0] = 0;
	leng[1] = len;
	hid_spec_lst = sdp_seq_alloc_with_length(dtds, values, leng, 2);
	hid_spec_lst2 = sdp_data_alloc(SDP_SEQ8, hid_spec_lst);
	sdp_attr_add(record, SDP_ATTR_HID_DESCRIPTOR_LIST, hid_spec_lst2);
}

/*
hid_attr_langs - array of uint16.
          size - how many elements in the array.
*/
//ATTR 0x0207
static void my_sdp_set_HID_LangIdBaseList(sdp_record_t *record, unsigned short *hid_attr_langs, int size) {
	void *dtds2[2];
	unsigned char dtd = SDP_UINT16;
	void *values2[2];
	sdp_data_t *lang_lst;
	sdp_data_t *lang_lst2;
	int i;

	for (i = 0; i < size; i++) {
		dtds2[i] = &dtd;
		values2[i] = &hid_attr_langs[i];
	}
	lang_lst = sdp_seq_alloc(dtds2, values2, size);
	lang_lst2 = sdp_data_alloc(SDP_SEQ8, lang_lst);
	sdp_attr_add(record, SDP_ATTR_HID_LANG_ID_BASE_LIST, lang_lst2);
}

//ATTR 0x0208
static void my_sdp_set_HID_SdpDisable(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_SDP_DISABLE, SDP_BOOL, &val);
}

//ATTR 0x0209
static void my_sdp_set_HID_BatteryPower(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_BATTERY_POWER, SDP_BOOL, &val);
}

//ATTR 0x020a
static void my_sdp_set_HID_RemoteWakeUp(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_REMOTE_WAKEUP, SDP_BOOL, &val);
}

//ATTR 0x020b
static void my_sdp_set_HID_ProfileVersion(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_PROFILE_VERSION, SDP_UINT16, &val);
}

//ATTR 0x020c
static void my_sdp_set_HID_SuperVersionTimeout(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_SUPERVISION_TIMEOUT, SDP_UINT16, &val);
}

//ATTR 0x020d
static void my_sdp_set_HID_NormallyConnectable(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_NORMALLY_CONNECTABLE, SDP_BOOL, &val);
}

//ATTR 0x020e
static void my_sdp_set_HID_BootDevice(sdp_record_t *record, unsigned int val) {
	sdp_attr_add_new(record, SDP_ATTR_HID_BOOT_DEVICE, SDP_BOOL, &val);
}



struct hidc_info_t {
	char *       service_name;
	char *       description;
	char *       provider;
	char *       desc_data; //USB HID Report Descriptor data
	unsigned int desc_len;
	unsigned int sub_class;
};

static unsigned short g_hid_attr_lang[] = {0x409, 0x100};

// With 0xffffffff, we get assigned the first free record >= 0x10000
// Make HID service visible (add to PUBLIC BROWSE GROUP)
sdp_record_t *SdpCreateRecord(unsigned int handle) {
	sdp_record_t *record = calloc(1, sizeof(sdp_record_t));
	record->handle = handle;
	return record;
}

void SdpDestroyRecord(sdp_record_t *record) {
	if (record) free(record);
}

sdp_record_t *SdpMakeUpHidRecord(sdp_record_t *record, struct hidc_info_t *hidc_info) {
	my_sdp_set_ServiceClassIDList(record);
	my_sdp_set_ProtocolDescriptorList(record);
	my_sdp_set_BrowseGroupList(record);
	my_sdp_set_LanguageBaseAttributeIDList(record);
	my_sdp_set_BluetoothProfileDescriptorList(record);
	my_sdp_set_AdditionalProtocolDescriptorLists(record);
	my_sdp_set_HID_DeviceReleaseNumber(record, 0x0100);
	my_sdp_set_HID_ParserVersion(record, 0x0111);
	my_sdp_set_HID_CountryCode(record, 0x21);
	my_sdp_set_HID_VirtualCable(record, 1);
	my_sdp_set_HID_ReconnectInitiate(record, 1);
	my_sdp_set_HID_LangIdBaseList(record, g_hid_attr_lang, ARRAY_SIZE(g_hid_attr_lang));
	my_sdp_set_HID_SdpDisable(record, 0);
	my_sdp_set_HID_BatteryPower(record, 1);
	my_sdp_set_HID_RemoteWakeUp(record, 1);
	my_sdp_set_HID_ProfileVersion(record, 0x0100);
	my_sdp_set_HID_SuperVersionTimeout(record, 0x0c80);
	my_sdp_set_HID_NormallyConnectable(record, 0);
	my_sdp_set_HID_BootDevice(record, 1);

	if (hidc_info->service_name)
		my_sdp_set_ServiceName(record, hidc_info->service_name);
	if (hidc_info->description)
		my_sdp_set_ServiceDescription(record, hidc_info->description);
	if (hidc_info->provider)
		my_sdp_set_ProviderName(record, hidc_info->provider);
	my_sdp_set_HID_DeviceSubClass(record, hidc_info->sub_class);
	my_sdp_set_HID_DescriptorList(record, hidc_info->desc_data, hidc_info->desc_len);
	return record;
}

int SdpRegisterRecord(sdp_record_t *record) {
	sdp_session_t *session;

	// Connect to SDP server on localhost, to publish service information
	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, 0);
	if (!session) {
		fprintf(stderr, "Failed to connect to SDP server: %s\n", strerror(errno));
		return 1;
	}

	// Submit our IDEA of a SDP record to the "sdpd"
	if (sdp_record_register(session, record, SDP_RECORD_PERSIST) < 0) {
		fprintf (stderr, "Service Record registration failed\n");
		return -1;
	}
	fprintf (stdout, "HID keyboard/mouse service registered. handle=0x%x\n", record->handle);

	return 0;
}

void SdpUnregisterRecord(unsigned int handle) {
	unsigned int	range=0x0000ffff;
	sdp_list_t    *	attr;
	sdp_session_t *	session;
	sdp_record_t  *	record;

	// Connect to the local SDP server
	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, 0);
	if (!session) return;

	attr = sdp_list_append(0, &range);
	record = sdp_service_attr_req(session, handle, SDP_ATTR_REQ_RANGE, attr);
	sdp_list_free(attr, 0);
	if (!record) {
		sdp_close(session);
		return;
	}
	sdp_device_record_unregister(session, BDADDR_ANY, record);
	sdp_close(session);
	return;
}

//==============================================================================
// Sample code
//==============================================================================

#define	HIDINFO_NAME	"Bluetooth HID Test"
#define	HIDINFO_PROV	"Huipeng Zhao"
#define	HIDINFO_DESC	"Test Bluetooth HID"

//  HID-Record for virtual keyboard and mouse device
static unsigned char g_hid_report_desc_moukbd[] = {
	0x05, 0x01,	// UsagePage GenericDesktop
	0x09, 0x02,	// Usage Mouse
	0xA1, 0x01,	// Collection Application
	0x85, 0x01,	// REPORT ID: 1
	0x09, 0x01,	// Usage Pointer
	0xA1, 0x00,	// Collection Physical
	0x05, 0x09,	// UsagePage Buttons
	0x19, 0x01,	// UsageMinimum 1
	0x29, 0x03,	// UsageMaximum 3
	0x15, 0x00,	// LogicalMinimum 0
	0x25, 0x01,	// LogicalMaximum 1
	0x75, 0x01,	// ReportSize 1
	0x95, 0x03,	// ReportCount 3
	0x81, 0x02,	// Input data variable absolute
	0x75, 0x05,	// ReportSize 5
	0x95, 0x01,	// ReportCount 1
	0x81, 0x01,	// InputConstant (padding)
	0x05, 0x01,	// UsagePage GenericDesktop
	0x09, 0x30,	// Usage X
	0x09, 0x31,	// Usage Y
	0x09, 0x38,	// Usage ScrollWheel
	0x15, 0x81,	// LogicalMinimum -127
	0x25, 0x7F,	// LogicalMaximum +127
	0x75, 0x08,	// ReportSize 8
	0x95, 0x02,	// ReportCount 3
	0x81, 0x06,	// Input data variable relative
	0xC0, 0xC0,	// EndCollection EndCollection
	0x05, 0x01,	// UsagePage GenericDesktop
	0x09, 0x06,	// Usage Keyboard
	0xA1, 0x01,	// Collection Application
	0x85, 0x02,	// REPORT ID: 2
	0xA1, 0x00,	// Collection Physical
	0x05, 0x07,	// UsagePage Keyboard
	0x19, 0xE0,	// UsageMinimum 224
	0x29, 0xE7,	// UsageMaximum 231
	0x15, 0x00,	// LogicalMinimum 0
	0x25, 0x01,	// LogicalMaximum 1
	0x75, 0x01,	// ReportSize 1
	0x95, 0x08,	// ReportCount 8
	0x81, 0x02,	// **Input data variable absolute
	0x95, 0x08,	// ReportCount 8
	0x75, 0x08,	// ReportSize 8
	0x15, 0x00,	// LogicalMinimum 0
	0x25, 0x65,	// LogicalMaximum 101
	0x05, 0x07,	// UsagePage Keycodes
	0x19, 0x00,	// UsageMinimum 0
	0x29, 0x65,	// UsageMaximum 101
	0x81, 0x00,	// **Input DataArray
	0xC0, 0xC0,	// EndCollection
};

struct hidc_info_t g_hidc_info = {
	.service_name = HIDINFO_NAME,
	.description  = HIDINFO_DESC,
	.provider     = HIDINFO_PROV,
	.desc_data    = (char*)&g_hid_report_desc_moukbd,
	.desc_len     = sizeof(g_hid_report_desc_moukbd),
	.sub_class    = 0x80,
};

static int sample_register_sdp_record(void) {
	sdp_record_t *record;
	record = (sdp_record_t *)SdpCreateRecord(0xffffffff);
	SdpMakeUpHidRecord(record, &g_hidc_info);
	SdpRegisterRecord(record);
	SdpDestroyRecord(record);
	return 0;
}
#if 0

int main(void) {
	sample_register_sdp_record();
	return 0;
}
#endif

