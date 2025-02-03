#include <Windows.h>
#include "Memory.h"
#include "StdLib.h"
#include "String.h"
#include "StringBuffer.h"
#include "Buffer.h"
#include "zlib.h"

typedef unsigned __int64 hash64_t;

class Hash64 {
private:
	static hash64_t	s_Table[256];
	enum : hash64_t {
		I_VAL = 0xffffffffffffffffui64,
	};
public:
	hash64_t Hash(const void* _data, int _size){
		LPCBYTE d;
		hash64_t hash;
		d = (LPCBYTE)_data;
		
		for (hash = I_VAL; _size > 0; _size--){
			hash = (hash >> 8) ^ s_Table[((DWORD)*(d++) ^ (DWORD)hash) & 0xFF];
		}
		return ~hash;
	}
	hash64_t HashLSN(const char* _data, int _size){
		LPCBYTE d;
		hash64_t hash;
		d = (LPCBYTE)_data;
		
		for (hash = I_VAL; _size > 0; _size--){
			hash = (hash >> 8) ^ s_Table[((DWORD)tolower(*(d++)) ^ (DWORD)hash) & 0xFF];
		}
		return ~hash;
	}
	hash64_t HashLSN(const char* _data){
		LPCBYTE d;
		hash64_t hash;
		d = (LPCBYTE)_data;
		
		for (hash = I_VAL; *d;){
			hash = (hash >> 8) ^ s_Table[((DWORD)tolower(*(char*)(d++)) ^ (DWORD)hash) & 0xFF];
		}
		return ~hash;
	}
	hash64_t HashSN(const char* _data, int _size){
		LPCBYTE d;
		hash64_t hash;
		d = (LPCBYTE)_data;
		
		for (hash = I_VAL; _size > 0; _size--){
			hash = (hash >> 8) ^ s_Table[((DWORD)(*(d++)) ^ (DWORD)hash) & 0xFF];
		}
		return ~hash;
	}
	hash64_t HashSN(const char* _data){
		LPCBYTE d;
		hash64_t hash;
		d = (LPCBYTE)_data;
		
		for (hash = I_VAL; *d;){
			hash = (hash >> 8) ^ s_Table[((DWORD)(*(d++)) ^ (DWORD)hash) & 0xFF];
		}
		return ~hash;
	}
};

hash64_t Hash64::s_Table[256] = {
    0x0000000000000000, 0x01A1561E0005800C, 0x0342AC3C000B0018, 0x02E3FA22000E8014, 0x0685587800160030, 0x07240E660013803C, 0x05C7F444001D0028, 0x0466A25A00188024, 
    0x0D0AB0F0002C0060, 0x0CABE6EE0029806C, 0x0E481CCC00270078, 0x0FE94AD200228074, 0x0B8FE888003A0050, 0x0A2EBE96003F805C, 0x08CD44B400310048, 0x096C12AA00348044, 
    0x1A1561E0005800C0, 0x1BB437FE005D80CC, 0x1957CDDC005300D8, 0x18F69BC2005680D4, 0x1C903998004E00F0, 0x1D316F86004B80FC, 0x1FD295A4004500E8, 0x1E73C3BA004080E4, 
    0x171FD110007400A0, 0x16BE870E007180AC, 0x145D7D2C007F00B8, 0x15FC2B32007A80B4, 0x119A896800620090, 0x103BDF760067809C, 0x12D8255400690088, 0x1379734A006C8084, 
    0x342AC3C000B00180, 0x358B95DE00B5818C, 0x37686FFC00BB0198, 0x36C939E200BE8194, 0x32AF9BB800A601B0, 0x330ECDA600A381BC, 0x31ED378400AD01A8, 0x304C619A00A881A4, 
    0x39207330009C01E0, 0x3881252E009981EC, 0x3A62DF0C009701F8, 0x3BC38912009281F4, 0x3FA52B48008A01D0, 0x3E047D56008F81DC, 0x3CE78774008101C8, 0x3D46D16A008481C4, 
    0x2E3FA22000E80140, 0x2F9EF43E00ED814C, 0x2D7D0E1C00E30158, 0x2CDC580200E68154, 0x28BAFA5800FE0170, 0x291BAC4600FB817C, 0x2BF8566400F50168, 0x2A59007A00F08164, 
    0x233512D000C40120, 0x229444CE00C1812C, 0x2077BEEC00CF0138, 0x21D6E8F200CA8134, 0x25B04AA800D20110, 0x24111CB600D7811C, 0x26F2E69400D90108, 0x2753B08A00DC8104, 
    0x6855878001600300, 0x69F4D19E0165830C, 0x6B172BBC016B0318, 0x6AB67DA2016E8314, 0x6ED0DFF801760330, 0x6F7189E60173833C, 0x6D9273C4017D0328, 0x6C3325DA01788324, 
    0x655F3770014C0360, 0x64FE616E0149836C, 0x661D9B4C01470378, 0x67BCCD5201428374, 0x63DA6F08015A0350, 0x627B3916015F835C, 0x6098C33401510348, 0x6139952A01548344, 
    0x7240E660013803C0, 0x73E1B07E013D83CC, 0x71024A5C013303D8, 0x70A31C42013683D4, 0x74C5BE18012E03F0, 0x7564E806012B83FC, 0x77871224012503E8, 0x7626443A012083E4, 
    0x7F4A5690011403A0, 0x7EEB008E011183AC, 0x7C08FAAC011F03B8, 0x7DA9ACB2011A83B4, 0x79CF0EE801020390, 0x786E58F60107839C, 0x7A8DA2D401090388, 0x7B2CF4CA010C8384, 
    0x5C7F444001D00280, 0x5DDE125E01D5828C, 0x5F3DE87C01DB0298, 0x5E9CBE6201DE8294, 0x5AFA1C3801C602B0, 0x5B5B4A2601C382BC, 0x59B8B00401CD02A8, 0x5819E61A01C882A4, 
    0x5175F4B001FC02E0, 0x50D4A2AE01F982EC, 0x5237588C01F702F8, 0x53960E9201F282F4, 0x57F0ACC801EA02D0, 0x5651FAD601EF82DC, 0x54B200F401E102C8, 0x551356EA01E482C4, 
    0x466A25A001880240, 0x47CB73BE018D824C, 0x4528899C01830258, 0x4489DF8201868254, 0x40EF7DD8019E0270, 0x414E2BC6019B827C, 0x43ADD1E401950268, 0x420C87FA01908264, 
    0x4B60955001A40220, 0x4AC1C34E01A1822C, 0x4822396C01AF0238, 0x49836F7201AA8234, 0x4DE5CD2801B20210, 0x4C449B3601B7821C, 0x4EA7611401B90208, 0x4F06370A01BC8204, 
    0xD0AB0F0002C00600, 0xD10A591E02C5860C, 0xD3E9A33C02CB0618, 0xD248F52202CE8614, 0xD62E577802D60630, 0xD78F016602D3863C, 0xD56CFB4402DD0628, 0xD4CDAD5A02D88624, 
    0xDDA1BFF002EC0660, 0xDC00E9EE02E9866C, 0xDEE313CC02E70678, 0xDF4245D202E28674, 0xDB24E78802FA0650, 0xDA85B19602FF865C, 0xD8664BB402F10648, 0xD9C71DAA02F48644, 
    0xCABE6EE0029806C0, 0xCB1F38FE029D86CC, 0xC9FCC2DC029306D8, 0xC85D94C2029686D4, 0xCC3B3698028E06F0, 0xCD9A6086028B86FC, 0xCF799AA4028506E8, 0xCED8CCBA028086E4, 
    0xC7B4DE1002B406A0, 0xC615880E02B186AC, 0xC4F6722C02BF06B8, 0xC557243202BA86B4, 0xC131866802A20690, 0xC090D07602A7869C, 0xC2732A5402A90688, 0xC3D27C4A02AC8684, 
    0xE481CCC002700780, 0xE5209ADE0275878C, 0xE7C360FC027B0798, 0xE66236E2027E8794, 0xE20494B8026607B0, 0xE3A5C2A6026387BC, 0xE1463884026D07A8, 0xE0E76E9A026887A4, 
    0xE98B7C30025C07E0, 0xE82A2A2E025987EC, 0xEAC9D00C025707F8, 0xEB688612025287F4, 0xEF0E2448024A07D0, 0xEEAF7256024F87DC, 0xEC4C8874024107C8, 0xEDEDDE6A024487C4, 
    0xFE94AD2002280740, 0xFF35FB3E022D874C, 0xFDD6011C02230758, 0xFC77570202268754, 0xF811F558023E0770, 0xF9B0A346023B877C, 0xFB53596402350768, 0xFAF20F7A02308764, 
    0xF39E1DD002040720, 0xF23F4BCE0201872C, 0xF0DCB1EC020F0738, 0xF17DE7F2020A8734, 0xF51B45A802120710, 0xF4BA13B60217871C, 0xF659E99402190708, 0xF7F8BF8A021C8704, 
    0xB8FE888003A00500, 0xB95FDE9E03A5850C, 0xBBBC24BC03AB0518, 0xBA1D72A203AE8514, 0xBE7BD0F803B60530, 0xBFDA86E603B3853C, 0xBD397CC403BD0528, 0xBC982ADA03B88524, 
    0xB5F43870038C0560, 0xB4556E6E0389856C, 0xB6B6944C03870578, 0xB717C25203828574, 0xB3716008039A0550, 0xB2D03616039F855C, 0xB033CC3403910548, 0xB1929A2A03948544, 
    0xA2EBE96003F805C0, 0xA34ABF7E03FD85CC, 0xA1A9455C03F305D8, 0xA008134203F685D4, 0xA46EB11803EE05F0, 0xA5CFE70603EB85FC, 0xA72C1D2403E505E8, 0xA68D4B3A03E085E4, 
    0xAFE1599003D405A0, 0xAE400F8E03D185AC, 0xACA3F5AC03DF05B8, 0xAD02A3B203DA85B4, 0xA96401E803C20590, 0xA8C557F603C7859C, 0xAA26ADD403C90588, 0xAB87FBCA03CC8584, 
    0x8CD44B4003100480, 0x8D751D5E0315848C, 0x8F96E77C031B0498, 0x8E37B162031E8494, 0x8A511338030604B0, 0x8BF04526030384BC, 0x8913BF04030D04A8, 0x88B2E91A030884A4, 
    0x81DEFBB0033C04E0, 0x807FADAE033984EC, 0x829C578C033704F8, 0x833D0192033284F4, 0x875BA3C8032A04D0, 0x86FAF5D6032F84DC, 0x84190FF4032104C8, 0x85B859EA032484C4, 
    0x96C12AA003480440, 0x97607CBE034D844C, 0x9583869C03430458, 0x9422D08203468454, 0x904472D8035E0470, 0x91E524C6035B847C, 0x9306DEE403550468, 0x92A788FA03508464, 
    0x9BCB9A5003640420, 0x9A6ACC4E0361842C, 0x9889366C036F0438, 0x99286072036A8434, 0x9D4EC22803720410, 0x9CEF94360377841C, 0x9E0C6E1403790408, 0x9FAD380A037C8404,
};

struct MNFSTHdr {
	size_32	Version;
	size_32 Offets[10];
};

const char c_map_lc_dot[28] = {
	0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, '.',
};

bool FindStrForHash(CStringBufferA &buf, hash64_t hash, int maxlen = 4){
	char lBuf[64], lBuf2[64], c;
	maxlen = min(maxlen, sizeof(lBuf)-1);
	zmem_t(lBuf);
	zmem_t(lBuf2);
	hash64_t lh;
	Hash64 h;
	int i;
	lBuf[0] = 0;
	for (;;){
		c = ++lBuf[0];
		lBuf2[0] = c_map_lc_dot[c];
		if (c == 28){
			lBuf[0] = 1;
			lBuf2[0] = c_map_lc_dot[1];
			for (i = 1; i < maxlen; i++){
				c = ++lBuf[i];
				lBuf2[i] = c_map_lc_dot[c];
				if (c < 28)
					break;
				lBuf[i] = 1;
				lBuf2[i] = c_map_lc_dot[1];
			}
			if (i >= maxlen)
				break;
		}
		lh = h.HashSN(lBuf2);
		if (lh == hash){
			buf = lBuf2;
			return true;
		}
	}
	return false;
}

class CEntry {
public:
	size_32	Count;
	class CSubEntry {
	public:
		size_32		Offset;
		size_32		Size;
		size_32		InflateDataOffset;
		CStringA	m_Name;
		CStringA	m_FullName;
		size_32		m_Extension;
		size_8		m_Value;
		hash64_t	m_hExtension;
	};
	CSubEntry*	m_pSubEntries;
	CEntry(){
		m_pSubEntries = NULL;
	}
	~CEntry(){
		if (m_pSubEntries)
			delete[] m_pSubEntries;
	}
};

class CExtensionMgr {
public:
	class CExtEntry {
	public:
		CStringA	Extension;
		hash64_t	Hash;
	};
	SList<CExtEntry> Entries;
	CStringBufferA Buf;
	const char* GetExtension(hash64_t hash){
		GenericSLNode<CExtEntry>* pNode;
		CExtEntry* pEntry;
		for (pNode = Entries.Get_Head(); pNode; pNode = pNode->m_pNext){
			if (pNode->m_pData->Hash == hash)
				return pNode->m_pData->Extension;
		}
		if (!FindStrForHash(Buf, hash, 5)){
			Buf.Print("%I64X", hash);
		}
		pEntry = new CExtEntry;
		if (pEntry){
			pEntry->Extension = Buf;
			pEntry->Hash = hash;
			Entries.Add_Tail(pEntry);
			return pEntry->Extension;
		}
		return "bin";		
	}
	void Add(const char* pExtension){
		hash64_t hash;
		Hash64 h;
		CExtEntry* pEntry;
		hash = h.HashSN(pExtension);
		pEntry = new CExtEntry;
		if (pEntry){
			pEntry->Extension = pExtension;
			pEntry->Hash = hash;
			Entries.Add_Tail(pEntry);
		}
	}
	void Clear(){
		Entries.Remove_All(SList<CExtEntry>::DefaultCallback);
	}
};

class CInflateStream {
public:
	z_stream Stream;
	CInflateStream(){
		zmem_t(Stream);
		inflateInit(&Stream);
	}
	~CInflateStream(){
		inflateEnd(&Stream);
	}
	void Reset(){
		inflateReset(&Stream);
	}
	operator z_stream*(){
		return &Stream;
	}
	z_stream* operator->(){
		return &Stream;
	}
};

void ReadAndUnzipPayload(file f, CDynMemoryBuffer &pl){
	char lBuf[8192];
	size_32 r;
	static CInflateStream stream;
	size_32 us;
	fread_t(f, &us);
	pl.EnsureSize(us);
	pl.SetSize(us);
	
	stream->next_out = (Bytef*)pl.GetBytePtr();
	stream->avail_out = us;
	int res;
	res = Z_OK;
	for (;stream->avail_out && res != Z_STREAM_END;){
		r = fread_(lBuf, sizeof(lBuf), f);
		stream->next_in = (Bytef*)lBuf;
		stream->avail_in = r;
		res = inflate(stream, Z_NO_FLUSH);
		if (res < 0)
			DebugBreak();
	}
	stream.Reset();
}

void ReadAndUnzipPointer(const void* p, size_32 size, CDynMemoryBuffer &pl){
	static CInflateStream stream;
	size_32 us;
	us = *(size_32*)p;
	p = ((size_32*)p)+1;
	pl.EnsureSize(us);
	pl.SetSize(us);
	stream->next_in = (Bytef*)p;
	stream->next_out = (Bytef*)pl.GetBytePtr();
	stream->avail_in = size-sizeof(size_32);
	stream->avail_out = us;
	int res = inflate(stream, Z_FINISH);
	stream.Reset();
}

CExtensionMgr emgr;

class CExtension {
public:
	CStringA	Extension;
	hash64_t	Hash;
	CExtension(const char* _extension){
		Hash64 h;
		Hash = h.HashSN(_extension);
		emgr.Add(_extension);
	}
};

#define E_EXT(_name) CExtension	E_##_name(#_name)

CExtension	E_ZGFX("zgfx");
CExtension	E_DDS("dds");

E_EXT(psystem);
E_EXT(glomm);
E_EXT(meshpack);
E_EXT(thrnode);
E_EXT(thrnodeop);
E_EXT(o3d);
E_EXT(lightdb);
E_EXT(tnode);
E_EXT(tnodeop);
E_EXT(physpack);

void ExtractInplaceOBPK(file f, const char* basefolder, bool zgfxtogfx, bool verbose, const char* pName){
	char magic[4];

	size_32 start;
	start = ftell_(f);
	fread_(magic, 4, f);
	if (memcmp(magic, "OBPK", 4) != 0)
		return;

	static CDynMemoryBuffer buf(8192), buf2(8192);
	size_32 dataoffset, datasize, ploffset;
	CDynMemoryBuffer::CDynMemoryReader r(&buf2);
	static CDynMemoryBuffer pl(8192);
	static CDynMemoryBuffer zgfx(8192);
	
	static CStringBufferA sb, sb2;
	

	size_32 a, b, c, d, e, g, h, j, k, l, m, n, o, p, idx;
	size_32 ls;
	size_8 s8;
	size_16* p16;
	hash64_t* ph;
	size_32 li, lj;
	char lBuf[1024];
	Hash64 hs;
	hash64_t ht;
	CEntry* pEntries;
	bool bl;
	hash64_t* peh;
	bool pfcompress;
	
	fseek_(f, start+0x11, SEEK_SET);
	fread_t(f, &dataoffset);
	fread_t(f, &datasize);
	fread_t(f, &ploffset);
	fread_t(f, &pfcompress);

	//Don't continue if it doesn't have the data with filesizes and offsets
	if (dataoffset == 0xFFFFFFFF)
		return;

	fseek_(f, start+0x1E, SEEK_SET); //Skip header

	fread_t(f, &a);
	fread_t(f, &b);
	fread_t(f, &c);
	fread_t(f, &d);
	buf.EnsureSize(c);
	fread_(buf.GetPtr(), c, f);
	//LogFormated(L"%X\n", ftell_(f));
	fread_t(f, &e);
	fread_t(f, &g);
	fread_t(f, &h);
	fread_t(f, &j);

	peh = (hash64_t*)buf.GetPtr();

	pEntries = new CEntry[a];

	idx = 0;

	for (li = 0; li < a; li++){
		//LogFormated(L"File %d:\n", li);
		fread_t(f, &k);
		fread_t(f, &l);
		fread_t(f, &m);
		pEntries[li].Count = l;
		pEntries[li].m_pSubEntries = new CEntry::CSubEntry[l];
		//LogFormated(L"Count = %X (%u)\n", l, l);
		ph = (hash64_t*)(buf.GetBytePtr()+m);
		for (lj = 0; lj < l; lj++){
			//LogFormated(L"Using hash %I64X\n", ph[lj]);
		}
		fread_t(f, &n);
		p16 = (size_16*)(buf.GetBytePtr()+n);
		for (lj = 0; lj < l; lj++){
			//LogFormated(L"Using size_16 %X (%u)\n", p16[lj], p16[lj]);
		}
		for (lj = 0; lj < l; lj++){
			fread_t(f, &s8);
			//LogFormated(L"Using size_8 %X (%u)\n", s8, s8);
			pEntries[li].m_pSubEntries[lj].m_Extension = s8 & ~0xE0; //Guesswork for the masking
			pEntries[li].m_pSubEntries[lj].m_Value = s8;
		}
		//LogFormated(L"\n");
	}
	fread_t(f, &bl);
	if (bl){
		fread_t(f, &ls);
		for (li = 0; li < a; li++){
			for (lj = 0; lj < pEntries[li].Count; lj++){
				fread_t(f, &ht);
				fread_t(f, &l);
				sb.Alloc(l+1);
				fread_(sb.GetBufferPtr(), l, f);
				sb()[l] = 0;
				sb2.Print("%s.%s", sb(), emgr.GetExtension(peh[pEntries[li].m_pSubEntries[lj].m_Extension]));
				pEntries[li].m_pSubEntries[lj].m_hExtension = peh[pEntries[li].m_pSubEntries[lj].m_Extension];
				pEntries[li].m_pSubEntries[lj].m_Name = sb;
				pEntries[li].m_pSubEntries[lj].m_FullName = sb2;
				//LogFormated(L"File %S = %I64X\n", sb2(), ht);
			}
		}
		if (dataoffset != 0xFFFFFFFF){
			buf2.Alloc(datasize);
			fseek_(f, start+dataoffset, SEEK_SET);
			fread_(buf2.GetBytePtr(), datasize, f);
			p = 0;
			for (li = 0; li < a; li++){
				for (lj = 0; lj < pEntries[li].Count; lj++){
					r.Get(&o);
					r.Get(&l);
					/*if (l > sizeof(lBuf)){
						k = ftell_(f);
						DebugBreak();
					}*/
					r.SeekDelta((ssize_32)l);

					pEntries[li].m_pSubEntries[lj].Offset = pfcompress ? 0 : p;
					pEntries[li].m_pSubEntries[lj].Size = o;
					p += o;
					//LogFormated(L"File %S = %I64X\n", sb2(), ht);
				}
			}
			for (li = 0; li < a; li++){
				for (lj = 0; lj < pEntries[li].Count; lj++){
					r.Get(&o);

					pEntries[li].m_pSubEntries[lj].InflateDataOffset = o;
				}
			}


			fseek_(f, start+ploffset, SEEK_SET);
			if (!pfcompress)
				ReadAndUnzipPayload(f, pl);
			file fo;
			CEntry::CSubEntry *ps;
			for (li = 0; li < a; li++){
				for (lj = 0; lj < pEntries[li].Count; lj++){
					ps = &pEntries[li].m_pSubEntries[lj];
					if (pfcompress){
						fseek_(f, start+ps->InflateDataOffset, SEEK_SET);
						ReadAndUnzipPayload(f, pl);
					}
					void* d = pl.GetBytePtr()+ps->Offset;
					if (ps->m_hExtension == E_ZGFX.Hash && zgfxtogfx){
						sb.Print("%s\\%s.gfx", basefolder, ps->m_Name());
						ReadAndUnzipPointer(d, ps->Size, zgfx);
						fo = fopen_(sb, "w");
						if (fo != INVALID_FILE){
							fwrite_(zgfx.GetPtr(), zgfx.GetSize(), fo);
							fclose_(fo);
						}
					} else {
						sb.Print("%s\\%s", basefolder, ps->m_FullName());
						fo = fopen_(sb, "w");
						if (fo != INVALID_FILE){
							fwrite_(d, ps->Size, fo);
							fclose_(fo);
						}
					}
				}
			}

		}
	} /*else 
		printf("*/
	delete [] pEntries;
}

class CDS2Mount {
public:
	file m_hFile;
	~CDS2Mount(){
		fclose_(m_hFile);
	}
};

int GetPathTokenCountA(const char *pPath){
	int result = 0;
	BOOL had_char = FALSE;
	for (;;){
		switch (*pPath){
			case 0:
			case '/':
			case '\\':
				if (!had_char) break;
				result++;
				had_char = FALSE;
				break;
			default:
				had_char = TRUE;
		}
		if (*pPath == 0) break;
		pPath++;
	}
	return result;
}

char* strndup(const char *pStr, int len){
	char *pRes = (char*)pStr;
	int _len = 0;
	while (*(pRes++) && _len < len) _len++;
	len = (_len < len) ? _len : len;
	if (pStr[len-1] != 0) len++;
	pRes = (char*)g_pAllocator(_len = len);
	//while (--_len) *(pRes++) = *(pStr++);
	memcpy(pRes, pStr, _len);

	pRes[_len] = 0;
	return pRes;
}

char *GetPathTokenA(const char *pPath, int *pLength, int token, BOOL copy_token){
	int len = 0;
	int count = 0;
	BOOL had_char = FALSE, had_slash = TRUE;
	const char *pPtr = pPath;
	for (;;){
		switch (*pPath){
			case 0:
			case '/':
			case '\\':
				had_slash = TRUE;
				if (!had_char) break;
				if (count++ == token){
					*pLength = len;
					if (!copy_token) return (char*)pPtr;
					return strndup(pPtr, len);
				}
				had_char = FALSE;
				break;
			default:
				had_char = TRUE;
				len++;
				if (!had_slash) break;
				had_slash = FALSE;
				pPtr = pPath;
				len = 1;
		}
		if (*pPath == 0) break;
		pPath++;
	}
	return NULL;
}

bool MakeDirFromFilenameA(char* pBuf, int bufsize, const char *pBaseDir = NULL, const char *pFilename = NULL){
	int count, i;
	char *pTemp;
	int l;

	//Assume full path in pBuf if this is NULL
	if (pBaseDir){
		strcpy_s(pBuf, bufsize, pBaseDir);
		strcpy_s(pBuf, bufsize, "\\");
		strcpy_s(pBuf, bufsize,  pFilename);
	}

	count = GetPathTokenCountA(pBuf);

	if (count < 1 || count > 128) return false;
	if (count == 1) return true;

	for (i = (pBuf[0] && (pBuf[1] == ':')) ? 1 : 0; i < count-1; i++){
		pTemp = GetPathTokenA(pBuf, &l, i, FALSE);
		*(pTemp+l) = 0;
		if (!(CreateDirectoryA(pBuf, NULL)  || GetLastError() == ERROR_ALREADY_EXISTS)) return false;

		*(pTemp+l) = '\\';
	}
	return true;
}

bool MakeDirFromPathA(char* pBuf, int bufsize, const char *pBaseDir = NULL){
	int count, i;
	char *pTemp;
	int l;

	//Assume full path in pBuf if this is NULL
	if (pBaseDir){
		strcpy_s(pBuf, bufsize, pBaseDir);
		strcpy_s(pBuf, bufsize, "\\");
	}

	count = GetPathTokenCountA(pBuf);

	if (count < 1 || count > 128) return false;
	if (count == 1) return true;

	for (i = (pBuf[0] && (pBuf[1] == ':')) ? 1 : 0; i < count; i++){
		pTemp = GetPathTokenA(pBuf, &l, i, FALSE);
		*(pTemp+l) = 0;
		if (!(CreateDirectoryA(pBuf, NULL)  || GetLastError() == ERROR_ALREADY_EXISTS)) return false;

		*(pTemp+l) = '\\';
	}
	return true;
}

void DS2Extract(const char* pSourceDir, const char* pDstDir, bool zgfxtogfx, bool verbose){
	//OBPKTest();

	file f;
	CStringBufferA buf, buf2, buf3, temp;
	CONSOLE_SCREEN_BUFFER_INFO info;

	buf.Print("%s\\pc.mnfst", pSourceDir);

	f = fopen_(buf, "r");
	if (f == INVALID_FILE){
		printf("Failed to open \"%s\" for reading. Please ensure you specified the correct source path (typically Darksiders 2 dir\\media).\n", buf());
		return;
	}
	size_32 c, i, u, l, v, j;
	size_8 s8, l3, al, uc0, uc1;

	bool b1, b2;
	size_16 l2, us0, us1;
	hash64_t hash, h2;
	Hash64 h;
	MNFSTHdr hdr;
	fread_t(f, &hdr);
	fread_t(f, &c);
	fread_t(f, &u); //Longest string
	size_16 a[256];

	CDS2Mount* pMounts;

	CStringA* pPaths;
	pPaths = new CStringA[c];

	//Paths

	for (i = 0; i < c; i++){
		fread_t(f, &hash);
		fread_t(f, &l);
		buf.Alloc(l+1);
		fread_(buf.GetBufferPtr(), l, f);
		buf()[l] = 0;
		h2 = h.HashSN(buf);
		//LogFormated(L"%X (%d) %S : %I64X %I64X\n", i, i, buf(), hash, h2);
		buf2.Print("%s\\%s", pDstDir, buf());
		pPaths[i] = buf2;
		MakeDirFromPathA(buf2.GetBufferPtr(), buf2.GetBufferSize());
	}
	
	fread_t(f, &c);
	//LogFormated(L"\n\n");

	pMounts = new CDS2Mount[c];

	//upak references

	for (i = 0; i < c; i++){
		fread_t(f, &s8);
		fread_t(f, &l2);
		buf.Alloc(l2+1);
		fread_(buf.GetBufferPtr(), l2, f);
		//fread_t(f, &hash);
		buf()[l2] = 0;
		h2 = h.HashSN(buf);
		//LogFormated(L"%X (%d) %S : %I64X -> %.2X\n", i, i, buf(), h2, s8);
		buf2.Print("%s\\%s.upak", pSourceDir, buf());
		pMounts[i].m_hFile = fopen_(buf2, "r");
		if (pMounts[i].m_hFile == INVALID_FILE){
		printf("Failed to open \"%s\" for reading. Please ensure you specified the correct source path (typically Darksiders 2 dir\\media).\n", buf2());
			return;
		}
	}
	fread_t(f, &c);
	//LogFormated(L"\n\n");

	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	for (i = 0; i < c; i++){
		fread_t(f, &l3);

		buf.Alloc(l3+1);
		fread_(buf.GetBufferPtr(), l3, f);
		
		buf()[l3] = 0;		
		fread_t(f, &hash);
		h2 = h.HashSN(buf);
		fread_t(f, &us0);
		fread_t(f, &us1);//Folder
		fread_t(f, &al);
		//fread_(a, sizeof(size_16)*al, f);
		fseek_(f, sizeof(size_16)*al, SEEK_CUR); //Possibly dependencies
		fread_t(f, &uc0);
		fread_t(f, &uc1); //.upak index, valid if != 0xFF
		fread_t(f, &b1);
		fread_t(f, &b2);
		fread_t(f, &v); //Offset
		//LogFormated(L"%X (%d) %S : %I64X %I64X -> us0 = %X us1 = %X al = %X uc0 = %X uc1 = %X b1 = %X b2 = %X v = %X\n", i, i, buf(), hash, h2, us0, us1, al, uc0, uc1, b1, b2, v);
		/*if (al){
			for (j = 0; j < al; j++){
				LogFormated(L"%s%X", j > 0 ? L", " : L"",  a[j]);
			}
			LogFormated(L"\n");
		}*/
		if (uc1 != 0xFF){
			fseek_(pMounts[uc1].m_hFile, v, SEEK_SET);
			ExtractInplaceOBPK(pMounts[uc1].m_hFile, pPaths[us1], zgfxtogfx, verbose, buf);
		}
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), info.dwCursorPosition);
		temp.Print( "%.2f", (float)(100.0 / (double)c * (double)(i+1)));
		printf("%-6s%%", temp());
	}

	delete [] pMounts;
	fclose_(f);

}

void GetCurrentDir(CStringBufferA &buf){
	DWORD size;
	char lBuf[1];
	size = GetCurrentDirectoryA(1, lBuf);
	buf.Alloc(size);
	buf.SetOffset(size-1);
	GetCurrentDirectoryA(size, buf());
}

int main(int argc, char** argv, char** env){
	bool verbose, zgfx;
	CStringBufferA out, source;
	int i;
	if (argc < 2){
		printf("Darksiders 2 extraction tool.\nSyntax: DS2Extract.exe <source dir> [/o:outputdir] [/v] [/nz]\n");
		printf("/v = Verbose output\n");
		printf("/nz = Don't convert .zgfx to .gfx files\n");
		emgr.Clear();
		return 0;
	}
	source = argv[1];
	verbose = false;
	zgfx = true;
	GetCurrentDir(out);
	 
	for (i = 2; i < argc; i++){
		if (_stricmp("/v", argv[i]) == 0)
			verbose = true;
		else if (_stricmp("/z", argv[i]) == 0)
			zgfx = false;
		else if (_strnicmp("/o:", argv[i], 3) == 0)
			out = argv[i]+3;
	}
	DS2Extract(source, out, zgfx, verbose);
	emgr.Clear();
	return 0;
}
