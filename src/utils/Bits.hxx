/*!
 * \file bits.hxx
 * \author G. Christian
 * \brief Defines some macros to help in bitmasking
 * \details Each READx macro corresponds to the number whose
 * binary representation is \e x sequential true bits. For example,
 * READ8 corresponds to 11111111. The intended use is in extracting
 * a subset of bits from a number, possibly combined with a shift operation.
 * For example, to read 8 bits from a longword, starting at bit 6:
 * \code
 * unsigned subword = (longword >> 6) & READ8;
 * \endcode
 */
#ifndef BITS_HXX
#define BITS_HXX

#ifdef READ1
#error ("READ1 already defined, you'll need to change the name")
#endif			 	
#ifdef READ2 	
#error ("READ2 already defined, you'll need to change the name")
#endif			 	
#ifdef READ3 	
#error ("READ3 already defined, you'll need to change the name")
#endif			 	
#ifdef READ4 	
#error ("READ4 already defined, you'll need to change the name")
#endif			 	
#ifdef READ5 	
#error ("READ5 already defined, you'll need to change the name")
#endif			 	
#ifdef READ6 	
#error ("READ6 already defined, you'll need to change the name")
#endif			 	
#ifdef READ7 	
#error ("READ7 already defined, you'll need to change the name")
#endif			 	
#ifdef READ8 	
#error ("READ8 already defined, you'll need to change the name")
#endif			 	
#ifdef READ9 	
#error ("READ9 already defined, you'll need to change the name")
#endif			 	
#ifdef READ10	
#error ("READ10 already defined, you'll need to change the name")
#endif			 	
#ifdef READ11	
#error ("READ11 already defined, you'll need to change the name")
#endif			 	
#ifdef READ12	
#error ("READ12 already defined, you'll need to change the name")
#endif			 	
#ifdef READ13	
#error ("READ13 already defined, you'll need to change the name")
#endif			 	
#ifdef READ14	
#error ("READ14 already defined, you'll need to change the name")
#endif			 	
#ifdef READ15	
#error ("READ15 already defined, you'll need to change the name")
#endif			 	
#ifdef READ16	
#error ("READ16 already defined, you'll need to change the name")
#endif			 	
#ifdef READ17	
#error ("READ17 already defined, you'll need to change the name")
#endif			 	
#ifdef READ18	
#error ("READ18 already defined, you'll need to change the name")
#endif			 	
#ifdef READ19	
#error ("READ19 already defined, you'll need to change the name")
#endif			 	
#ifdef READ20	
#error ("READ20 already defined, you'll need to change the name")
#endif			 	
#ifdef READ21	
#error ("READ21 already defined, you'll need to change the name")
#endif			 	
#ifdef READ22	
#error ("READ22 already defined, you'll need to change the name")
#endif			 	
#ifdef READ23	
#error ("READ23 already defined, you'll need to change the name")
#endif			 	
#ifdef READ24	
#error ("READ24 already defined, you'll need to change the name")
#endif			 	
#ifdef READ25	
#error ("READ25 already defined, you'll need to change the name")
#endif			 	
#ifdef READ26	
#error ("READ26 already defined, you'll need to change the name")
#endif			 	
#ifdef READ27	
#error ("READ27 already defined, you'll need to change the name")
#endif			 	
#ifdef READ28	
#error ("READ28 already defined, you'll need to change the name")
#endif			 	
#ifdef READ29	
#error ("READ29 already defined, you'll need to change the name")
#endif			 	
#ifdef READ30	
#error ("READ30 already defined, you'll need to change the name")
#endif			 	
#ifdef READ31	
#error ("READ31 already defined, you'll need to change the name")
#endif			 	
#ifdef READ32	
#error ("READ32 already defined, you'll need to change the name")
#endif			 	
#ifdef READ33	
#error ("READ33 already defined, you'll need to change the name")
#endif			 	
#ifdef READ34	
#error ("READ34 already defined, you'll need to change the name")
#endif			 	
#ifdef READ35	
#error ("READ35 already defined, you'll need to change the name")
#endif			 	
#ifdef READ36	
#error ("READ36 already defined, you'll need to change the name")
#endif			 	
#ifdef READ37	
#error ("READ37 already defined, you'll need to change the name")
#endif			 	
#ifdef READ38	
#error ("READ38 already defined, you'll need to change the name")
#endif			 	
#ifdef READ39	
#error ("READ39 already defined, you'll need to change the name")
#endif			 	
#ifdef READ40	
#error ("READ40 already defined, you'll need to change the name")
#endif			 	
#ifdef READ41	
#error ("READ41 already defined, you'll need to change the name")
#endif			 	
#ifdef READ42	
#error ("READ42 already defined, you'll need to change the name")
#endif			 	
#ifdef READ43	
#error ("READ43 already defined, you'll need to change the name")
#endif			 	
#ifdef READ44	
#error ("READ44 already defined, you'll need to change the name")
#endif			 	
#ifdef READ45	
#error ("READ45 already defined, you'll need to change the name")
#endif			 	
#ifdef READ46	
#error ("READ46 already defined, you'll need to change the name")
#endif			 	
#ifdef READ47	
#error ("READ47 already defined, you'll need to change the name")
#endif			 	
#ifdef READ48	
#error ("READ48 already defined, you'll need to change the name")
#endif			 	
#ifdef READ49	
#error ("READ49 already defined, you'll need to change the name")
#endif			 	
#ifdef READ50	
#error ("READ50 already defined, you'll need to change the name")
#endif			 	
#ifdef READ51	
#error ("READ51 already defined, you'll need to change the name")
#endif			 	
#ifdef READ52	
#error ("READ52 already defined, you'll need to change the name")
#endif			 	
#ifdef READ53	
#error ("READ53 already defined, you'll need to change the name")
#endif			 	
#ifdef READ54	
#error ("READ54 already defined, you'll need to change the name")
#endif			 	
#ifdef READ55	
#error ("READ55 already defined, you'll need to change the name")
#endif			 	
#ifdef READ56	
#error ("READ56 already defined, you'll need to change the name")
#endif			 	
#ifdef READ57	
#error ("READ57 already defined, you'll need to change the name")
#endif			 	
#ifdef READ58	
#error ("READ58 already defined, you'll need to change the name")
#endif			 	
#ifdef READ59	
#error ("READ59 already defined, you'll need to change the name")
#endif			 	
#ifdef READ60	
#error ("READ60 already defined, you'll need to change the name")
#endif			 	
#ifdef READ61	
#error ("READ61 already defined, you'll need to change the name")
#endif			 	
#ifdef READ62	
#error ("READ62 already defined, you'll need to change the name")
#endif			 	
#ifdef READ63	
#error ("READ63 already defined, you'll need to change the name")
#endif			 	
#ifdef READ64	
#error ("READ64 already defined, you'll need to change the name")
#endif


#define READ1		0x1
#define READ2		0x3
#define READ3		0x7
#define READ4		0xf
#define READ5		0x1f
#define READ6		0x3f
#define READ7		0x7f
#define READ8		0xff
#define READ9		0x1ff
#define READ10	0x3ff
#define READ11	0x7ff
#define READ12	0xfff
#define READ13	0x1fff
#define READ14	0x3fff
#define READ15	0x7fff
#define READ16	0xffff
#define READ17	0x1ffff
#define READ18	0x3ffff
#define READ19	0x7ffff
#define READ20	0xfffff
#define READ21	0x1fffff
#define READ22	0x3fffff
#define READ23	0x7fffff
#define READ24	0xffffff
#define READ25	0x1ffffff
#define READ26	0x3ffffff
#define READ27	0x7ffffff
#define READ28	0xfffffff
#define READ29	0x1fffffff
#define READ30	0x3fffffff
#define READ31	0x7fffffff
#define READ32	0xffffffff
#define READ33	0x1ffffffff
#define READ34	0x3ffffffff
#define READ35	0x7ffffffff
#define READ36	0xfffffffff
#define READ37	0x1fffffffff
#define READ38	0x3fffffffff
#define READ39	0x7fffffffff
#define READ40	0xffffffffff
#define READ41	0x1ffffffffff
#define READ42	0x3ffffffffff
#define READ43	0x7ffffffffff
#define READ44	0xfffffffffff
#define READ45	0x1fffffffffff
#define READ46	0x3fffffffffff
#define READ47	0x7fffffffffff
#define READ48	0xffffffffffff
#define READ49	0x1ffffffffffff
#define READ50	0x3ffffffffffff
#define READ51	0x7ffffffffffff
#define READ52	0xfffffffffffff
#define READ53	0x1fffffffffffff
#define READ54	0x3fffffffffffff
#define READ55	0x7fffffffffffff
#define READ56	0xffffffffffffff
#define READ57	0x1ffffffffffffff
#define READ58	0x3ffffffffffffff
#define READ59	0x7ffffffffffffff
#define READ60	0xfffffffffffffff
#define READ61	0x1fffffffffffffff
#define READ62	0x3fffffffffffffff
#define READ63	0x7fffffffffffffff
#define READ64	0xffffffffffffffff

#endif
