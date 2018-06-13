/*
 * Dcs.cpp
 * 
 * Copyright 2016 Robert Chadelas <robert@clravirmdaq01.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
//====================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

#include "Tools.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <netinet/in.h>
#include <endian.h>
#include "frame.h"
#define VERSION 0.1 

#define ConvLong(a,b,c,d) (((unsigned long)(a)<<48) + ((unsigned long)(b)<<32) + ((unsigned long)(c)<<16) + (unsigned long)(d))


struct param {
	bool WriteTimesTamp = false;
	bool DecodeChannel  = false;
	bool Histo = false;
	bool LostFrame = false;
	string FeId;
	string DirName;
	string FileName;
	char feid;
	
};
	

struct Stats {
	long NbFrameRecorded;
	long ShortFrame;
	long LongFrame;
	long ErrHeader;
	long ErrFrontEnd;
	long LostFrameAsm;
	unsigned long FrameAsmold;
	unsigned int FrameAmcold;
	unsigned long TimeStampold;
	std::vector <unsigned int> NbFrameAmc;
	std::vector <unsigned long> NbFrameAsm;
	std::vector <unsigned long> TimestampAsm;
	std::vector <unsigned long> TimeStampThor;
	std::vector <unsigned long> TimeStampTrigThorAsm;
	std::vector <long> TimeStampRel;
	unsigned long NoTriggerReceived; 
	std::vector <bool> TrigTimeout;
	std::vector <unsigned long> NbTriggerThor;
	std::vector <unsigned long> TimeStamp_Thor_Asm;
	std::vector <unsigned int> CptTriggerThor;
	std::vector <unsigned int> CptTriggerAsm;
	std::vector <unsigned int> NbframeAmcRec;
	unsigned long ShortFragment;
	unsigned long LongFragment;
};

unsigned long Histo[24][4096];
unsigned long oneFrameLost;
unsigned long NbframeRec;
int verbose=0;

bool DecodeFragment(unsigned short *Fragment,unsigned int lg,struct Stats *S,
						  unsigned short NbSample,struct param *p)
/**************************************************************************************
 * 
 * 
 * ************************************************************************************/
{
	struct sPacketFile	*Frame = (struct sPacketFile *) Fragment;
	struct S_HeaderFrame *HeaderFrame=  &Frame->FrameHeader; //Fragment;
	
	string Name0,Name1,Name2,Name3;
	unsigned short *FragCh0,*FragCh1,*FragCh2,*FragCh3;

//	unsigned short HeaderFrame = ntohs(Fragment[0]);
	
	unsigned short EndFrame  = ntohs(Fragment[(lg/2)-1]);
	unsigned short CrcFrame  = ntohs(Fragment[(lg/2)-2]);
	unsigned short EndFrame1 = ntohs(Fragment[((lg-8)/2)-1]);
//	unsigned short CrcFrame1 = ntohs(Fragment[((lg-7)/2)-1]);
	unsigned short EndFrame2 = ntohs(Fragment[((lg+8)/2)-1]);
//	unsigned short CrcFrame2 = ntohs(Fragment[((lg+7)/2)-1]);
	
	unsigned short FeId		 = (ntohs(HeaderFrame->FeIdK30)  & 0x7f00) >> 8;//Fragment[3]);

//	unsigned short FirstCh   = ntohs(*(Fragment+(sizeof(struct S_HeaderFrame)/2)));
	if (p->DecodeChannel) {
		FragCh0	 = &(Fragment[21]);
		FragCh1	 = &Fragment[21+NbSample+12];
		FragCh2	 = &Fragment[21+2*(NbSample+12)];
		FragCh3	 = &Fragment[21+3*(NbSample+12)];
/*		cout << hex << "Channel \t" << Fragment[21] << "\t" << Fragment[21+NbSample+12] << "\t" 
			  << Fragment[21+2*(NbSample+12)] << "\t" << Fragment[21+3*(NbSample+12)] << dec << endl;
			  
		cout << hex << "valfrag \t" << ntohs(Fragment[22]) << "\t" << ntohs(Fragment[22+NbSample+12]) << "\t" 
			  << ntohs(Fragment[22+2*(NbSample+12)]) << "\t" << ntohs(Fragment[22+3*(NbSample+12)]) << dec << endl;
			  
		cout << hex << "valptr  \t" << ntohs(FragCh0[512]) << "\t" << ntohs(FragCh1[512]) << "\t" 
			  << ntohs(FragCh2[512]) << "\t" << ntohs(FragCh3[512]) << dec << endl;
*/			  
		Name0 = "/datas/" + p->DirName + "/Ch" + to_string((ntohs(*FragCh0)& 0x7f00) >> 8);	
		Name1 = "/datas/" + p->DirName + "/Ch" + to_string((ntohs(*FragCh1)& 0x7f00) >> 8);
		Name2 = "/datas/" + p->DirName + "/Ch" + to_string((ntohs(*FragCh2)& 0x7f00) >> 8);
		Name3 = "/datas/" + p->DirName + "/Ch" + to_string((ntohs(*FragCh3)& 0x7f00) >> 8);
	}


//	cout << "First Ch " <<  NbSample << "  " << hex << ((ntohs(*FragCh0)& 0x7f00) ) << " " << ((ntohs(*FragCh1)& 0x7f00) >> 8) << " " << ntohs(*FragCh2) << " " << ntohs(*FragCh3) << dec<< endl;
	unsigned long tempsmsb1 = ntohs(HeaderFrame->TimeStampAsmMsb);
	unsigned long tempsmsb2 = ntohs(HeaderFrame->TimeStampAsmOsb);
	unsigned long tempslsb2 = ntohs(HeaderFrame->TimeStampAsmUsb);
	unsigned long tempslsb1 = ntohs(HeaderFrame->TimeStampAsmLsb);
	unsigned long temp  = (tempsmsb1 << 48) | (tempsmsb2 << 32) | (tempslsb2 << 16) | tempslsb1;

	unsigned long tstpthormsb = ntohs(HeaderFrame->ThorTrigTimeStampMsb);
	unsigned long tstpthorosb = ntohs(HeaderFrame->ThorTrigTimeStampOsb);
	unsigned long tstpthorlsb = ntohs(HeaderFrame->ThorTrigTimeStampLsb);
	unsigned long tstpthor = (tstpthormsb << 32) | (tstpthorosb << 16) | tstpthorlsb;
	
	S->TimeStampThor.push_back(tstpthor);
	
	unsigned short TT = ntohs(HeaderFrame->ThorTT);
	
	unsigned long TimeStamp_Thor_Asm_Msb = ntohs(HeaderFrame->TimeStampTrigThorAsmMsb);
	unsigned long TimeStamp_Thor_Asm_Osb = ntohs(HeaderFrame->TimeStampTrigThorAsmOsb);
	unsigned long TimeStamp_Thor_Asm_Usb = ntohs(HeaderFrame->TimeStampTrigThorAsmUsb);
	unsigned long TimeStamp_Thor_Asm_Lsb = ntohs(HeaderFrame->TimeStampTrigThorAsmLsb);
	
	
//	unsigned long TimeStamp_Thor_Asm_64bit = 	(TimeStamp_Thor_Asm_Msb << 48) + (TimeStamp_Thor_Asm_Osb << 32) + 
//															(TimeStamp_Thor_Asm_Usb << 16) + TimeStamp_Thor_Asm_Lsb;
														
	unsigned long TimeStamp_Thor_Asm_64bit = ConvLong(TimeStamp_Thor_Asm_Msb,TimeStamp_Thor_Asm_Osb , 
															TimeStamp_Thor_Asm_Usb , TimeStamp_Thor_Asm_Lsb);	
	
//	cout << hex << TimeStamp_Thor_Asm_Msb << " " << TimeStamp_Thor_Asm_Osb << " " << TimeStamp_Thor_Asm_Usb << " " << TimeStamp_Thor_Asm_Lsb 
//			<<  "   " << TimeStamp_Thor_Asm_64bit << dec << endl;
	
	S->TimeStamp_Thor_Asm.push_back(TimeStamp_Thor_Asm_64bit);
	

	S->CptTriggerThor.push_back(ntohs(HeaderFrame->CptTriggerThorMsb) <<16 | ntohs(HeaderFrame->CptTriggerThorLsb));
	S->CptTriggerAsm.push_back(ntohs(HeaderFrame->CptTriggerAsmMsb) <<16 | ntohs(HeaderFrame->CptTriggerAsmLsb));
	
	if (S->TimestampAsm.size() >0) {
		S->TimeStampRel.push_back(temp - S->TimestampAsm.back());
		// - S->TimeStampThor.back());
	}
	else {
		  S->TimeStampRel.push_back(0);
	   }
		
	S->TimestampAsm.push_back(temp);

	S->NbFrameAmc.push_back((ntohs(HeaderFrame->NbFrameAmcMsb) << 16) + (ntohs(HeaderFrame->NbFrameAmcLsb)));
	//cout << "amc " <<hex <<  (ntohs(HeaderFrame->NbFrameAmcMsb)) << (ntohs(HeaderFrame->NbFrameAmcLsb)) << endl;
	

	if (ntohs(HeaderFrame->StartOfFrame) != 0x1230) {
		cout << FgColor::red << "Header frame error 0x" << hex << ntohs(HeaderFrame->StartOfFrame) << dec << FgColor::white << endl;
		S->ErrHeader++;
	}

	

	if (FeId != p->feid ) {
		S->ErrFrontEnd++;
		S->NbFrameAsm.push_back(0xffffffffffffffff);
		printf("FeId = %x ReqFeid=%x\n",FeId,p->feid);
		 //mettre le numero de frond end
	}
	else {

	unsigned long TempNbFrame = ConvLong(ntohs(HeaderFrame->NbFrameAsmMsb),ntohs(HeaderFrame->NbFrameAsmOsb), 
													ntohs(HeaderFrame->NbFrameAsmUsb),ntohs(HeaderFrame->NbFrameAsmLsb));
	S->NbFrameAsm.push_back(TempNbFrame);
	if ((TT & 0x8000) == 0x8000) {
//		cout << FgColor::yellow << "frame Number : \t" << TempNbFrame  <<"\tErreur TT " << hex << TT << dec << endl;
		S->NoTriggerReceived++;
		S->TrigTimeout.push_back(true);
	}
	else S->TrigTimeout.push_back(false);

	
	if (S->CptTriggerThor.back() != S->CptTriggerAsm.back()) {
//		cout << FgColor::blue << "frame Number : \t" << TempNbFrame  << "\t" << PosFileabs <<"\tErreur Counter trigger " 
//		<< S->CptTriggerThor.back() << "\t" << S->CptTriggerAsm.back() <<dec << endl;
	}
										
		if (S->NbFrameAsm.size() > 1) {
			
			if (S->FrameAsmold+1 != S->NbFrameAsm.back()) {
				if ((S->NbFrameAsm.back() - (S->FrameAsmold+1)) == 1) oneFrameLost++;
//				cout.width(8);
//				cout <<  FgColor::red << hex << "FeId  " << FeId << dec 
//					<< "  old " << S->FrameAsmold << " New = " << S->NbFrameAsm.back()  
//				   <<  "\tDiff = " << S->NbFrameAsm.back() - (S->FrameAsmold+1) 
//				   << "\told Amc " << S->FrameAmcold << "\tNew Amc =  " << S->NbFrameAmc.back()
//				   << "\tTimeStamp diff " << ((S->TimestampAsm.back() - (S->TimeStampold))*6.6)/1000 << "\tus" << dec  
//				   << "\tTimeRel " << (S->TimestampAsm.back() - S->TimestampAsm.at(0))*6.66/1000000 << "\tms"
//				   << FgColor::white << endl;

				S->LostFrameAsm += S->NbFrameAsm.back() - (S->FrameAsmold+1);
			  
			  	if (p->LostFrame) {

					string NameLostFrame = p->DirName + "/LostFrame" + to_string(FeId) + ".txt";	
					std::ofstream FileLostFrame (NameLostFrame.c_str(),ios::app);
					FileLostFrame.precision(10);
					FileLostFrame  << S->NbFrameAsm.back() << "\t"
					<< (S->TimestampAsm.back() - S->TimestampAsm.at(0))*6.66/1000000000 << "\t" 
					<< S->NbFrameAsm.back() - (S->FrameAsmold+1)  << "\t"
					<< S->NbFrameAsm.back()*8230 << endl;
				}

		  }
		  else
		  { 
			//cout <<" Ch = " << ((FirstCh & 0x7f00) >>8) << "  TimeStamp diff " << ((S->Timestamp.back() - S->TimeStampold)*6.6) << "  ns" << FgColor::white << endl;
		  }
		}
		S->FrameAsmold = S->NbFrameAsm.back(); 
		S->FrameAmcold = S->NbFrameAmc.back(); 
		S->TimeStampold = S->TimestampAsm.back();
	}
	
	S->NbFrameRecorded++;

	if (((EndFrame & 0x10ff) == 0x10fb)  && (CrcFrame == 0x9876))	{
//		cout << "end frame = " << hex <<EndFrame << dec << endl;
	}
	else {
		cout << dec << "NumFrameAmc " << S->NbFrameAmc.back() << dec << "   ";
		cout << FgColor::yellow << "End frame error " << hex << EndFrame << " -8 " << EndFrame1 << " +8 " << EndFrame2 << dec  << FgColor::white << endl;
		return false;
	}
	unsigned int size = NbSample+11;
	if (p->Histo) {
		for (unsigned int k=1;k<=size;k++) {
			Histo[((ntohs(*FragCh0) & 0x7f00) >> 8)][ntohs(FragCh0[k]) & 0xfff]++;
			Histo[((ntohs(*FragCh1) & 0x7f00) >> 8)][ntohs(FragCh1[k]) & 0xfff]++;
			Histo[((ntohs(*FragCh2) & 0x7f00) >> 8)][ntohs(FragCh2[k]) & 0xfff]++;
			Histo[((ntohs(*FragCh3) & 0x7f00) >> 8)][ntohs(FragCh3[k]) & 0xfff]++;
//			if (k==size-1) cout << "last sample = " << (ntohs(FragCh3[k]) & 0xfff) << endl;
		}
	}

	
	if (p->DecodeChannel) {
		std::ofstream file0 (Name0.c_str(),ios::app);
		std::ofstream file1 (Name1.c_str(),ios::app);
		std::ofstream file2 (Name2.c_str(),ios::app);
		std::ofstream file3 (Name3.c_str(),ios::app);
		unsigned short FragmentCh0[size],FragmentCh1[size],FragmentCh2[size],FragmentCh3[size];
		for (unsigned int k=0;k<=size;k++) {
			FragmentCh0[k] = ntohs(FragCh0[k]) & 0xfff;
			FragmentCh1[k] = ntohs(FragCh1[k]) & 0xfff;
			FragmentCh2[k] = ntohs(FragCh2[k]) & 0xfff;
			FragmentCh3[k] = ntohs(FragCh3[k]) & 0xfff;
		}
		file0.write((const char *) &FragmentCh0[1],size);
		file1.write((const char *) &FragmentCh1[1],size);
		file2.write((const char *) &FragmentCh2[1],size);
		file3.write((const char *) &FragmentCh3[1],size);
	}
/*	file0.close();
	file1.close();
	file2.close();
	file3.close();
*/
//	cout << hex << "NumFrameAmc " << (ntohs(Fragment[1]) << 16) + (ntohs(Fragment[2])) << dec << endl;
	return true;
	
}

string HextoString(int x) {
	std::stringstream ss;
	ss << hex << x;
   return(ss.str());
}



void ReadFile(struct param *p) 
/*********************************************************************************************************************************************/
/*																																															*/
/*																																															*/
/*********************************************************************************************************************************************/
{
	std::ofstream HistoFile[24];
	Stats Statistic;
	unsigned short FeId;
	Statistic.NbFrameRecorded = 0;
	Statistic.ShortFrame = 0;
	Statistic.LongFrame = 0;
	Statistic.ErrHeader = 0;
	Statistic.ErrFrontEnd = 0;
	Statistic.LostFrameAsm = 0;
	Statistic.NoTriggerReceived = 0;
	Statistic.ShortFragment=0;
	Statistic.LongFragment=0;
	
	struct S_HeaderFrame *DataHeaderFrame;
	struct sPacketFile PacketFile;

	for (int ch=0;ch<24;ch++)
		for (int i=0;i<4096;i++)
			Histo[ch][i] = 0;

	

	
	unsigned short *Fragment; 
	std::streampos PosFileabs;
		
	std::ifstream file(p->FileName.c_str(),ios::binary);
	file.seekg(0,file.end);
	unsigned int lenFile = file.tellg();
	cout << "Sizeof file " << p->FileName << " : " << lenFile << " Bytes \t SizeOf HederFrame : " << sizeof(struct sPacketFile) << endl;
	file.seekg(0,file.beg);

	if (file.is_open() && (lenFile > 100)) {
		file.seekg(24);   //header file Pcap => to decode
		file.read((char*) &PacketFile, sizeof(struct sPacketFile));
		DataHeaderFrame = &PacketFile.FrameHeader;
		FeId = (ntohs(PacketFile.FrameHeader.FeIdK30)  & 0x7f00) >> 8;
		cout << FgColor::green << "FeId = " << hex << FeId << dec << endl;
		//file.read((char*) &DataFile, sizeof(struct S_HeaderFrame));
	}
	else {
		cout << FgColor::red << "Opening Error file or no datas avaiables  : " << p->FileName << FgColor::white << endl;
		return;
	}
 	
 	if (p->feid == -1) {
		p->feid = FeId;
	}
 	
 	unsigned short NbSample = ntohs(DataHeaderFrame->NbSample);

	int LenFrame8Bits =  (((NbSample*2)+4)*4)+sizeof(struct sPacketFile)+4;//((NbSample*2)*4)+sizeof(struct S_HeaderFrame)*2+24+2; //+24 à voir et +2 c'est pour le CRC et fin de trame;
	
	cout  << "Nbsamples \t" << NbSample << "  Len Frame " << LenFrame8Bits << endl;
	file.seekg(24);
	int iteration =0;
	Fragment = new unsigned short[(LenFrame8Bits)/2];   //Allocate memory fragment Word 16 bits
	bool FoundFragment=false;
	while (file.good()) {
		
		iteration++;
		if (Fragment) {
			file.read((char *) Fragment,LenFrame8Bits);
			struct sPacketFile	*Frame = (struct sPacketFile *) Fragment;
			struct S_HeaderFrame *HeaderFrame=  &Frame->FrameHeader; //Fragment;

			FoundFragment = false;
			unsigned short EndFrame  = ntohs(Fragment[(LenFrame8Bits/2)-1]);
			unsigned short StartOfFrame = ntohs(HeaderFrame->StartOfFrame);
//			unsigned short FeId		= (ntohs(HeaderFrame->FeIdK30)  & 0x7f00) >> 8;//Fragment[3]);
			unsigned short K30		= (ntohs(HeaderFrame->FeIdK30)  & 0x00ff);
			
			if ((K30 == 0xfe) && (StartOfFrame == 0x1230) && ((EndFrame & 0x10ff) == 0x10fb)) {
				if (!DecodeFragment(Fragment,LenFrame8Bits,&Statistic,NbSample,p)) {
					cout << "Fragment num = " << iteration << endl;break;
				}
				if (iteration == 1) 
					cout 	<< FgColor::green <<"Time Stamp 0  " << Statistic.TimestampAsm.at(0)*6.66/1000000 << "ms\t" 
							<< Statistic.NbFrameAsm.at(0) << FgColor::white << endl;
			} else {   //! Erreur de Fragment ==> Recherche le debut du nouveau fragment 
				cout << FgColor::red << "Erreur Fragment " << endl;
				for (int index=50;index < (((LenFrame8Bits)/2)-1);index++) {
					if ((ntohs (Fragment[index]) == 0x9876) && ((ntohs(Fragment[index+1]) & 0x10ff) == 0x10fb)) {
						cout << FgColor::yellow << "Position file " << (index*2-LenFrame8Bits+4) << "  " << index << " " 
								<< hex << ntohs (Fragment[index]) << " " << ntohs(Fragment[index+1]) << dec << endl;
						file.seekg((index*2 - LenFrame8Bits+4),ios::cur);
						Statistic.ShortFragment++;
						FoundFragment = true;
						break;
					}
				}
				if (!FoundFragment) {
					cout << FgColor::yellow << "End Position Fragment not found " << endl;
					unsigned int iter=0;
					cout << "file pos cur " << file.tellg() << endl;
					while (file.good()) { 
						unsigned short Temp[2];
						file.read((char *) &Temp,4);
						iter++;
						if (((ntohs(Temp[0]) == 0x9876))) {
							if ((ntohs(Temp[1]) & 0x10ff) == 0x10fb) 
							{
								cout << FgColor::yellow << "New Fragment Found " << iter*2 << " " << hex << ntohs(Temp[0]) << " " << (ntohs(Temp[1]) & 0x10ff) << dec << endl;
								Statistic.LongFragment++;
								FoundFragment = true;
								break;
							}
						}
					}
				}
				// todo Rechercher le fragment valid suivant
			}
			
		}
		else cout << "Error Allocation memory " << endl;
	}
	delete Fragment;
	
	if (p->WriteTimesTamp) {
		string NameTstp = "/datas/" + p->DirName + "/TimeStamp" + p->FeId + ".txt";
		cout << "Write timeStamp file " <<  NameTstp << endl;
		ofstream fileTstp (NameTstp,std::ofstream::app);
		if (fileTstp.is_open()) {
			cout 	<< "size = " << Statistic.TimestampAsm.size() << "\t" << Statistic.NbFrameAsm.size() << "\t" << Statistic.TimeStamp_Thor_Asm.size() 
					<< "\tThor size " <<  Statistic.TimeStampThor.size() << "\tTimeout" << Statistic.TrigTimeout.size() << "\t "
					<< "trig size " << Statistic.CptTriggerThor.size() << endl;

//			fileTstp << "#A\tB\tC\tD\tE\tF\tG\tH\tI\tJ\tK\t" << p->FileName << endl;
			for (unsigned int i=0;i<Statistic.TimestampAsm.size();++i) {

				double deltaThorAsm = (double) Statistic.TimestampAsm.at(i)*6.66 - (double) Statistic.TimeStampThor.at(i)*6.66;
				
				fileTstp <<  NbframeRec++ << "\t"
							<<  Statistic.NbFrameAmc.at(i) << "\t"				
							<<  Statistic.NbFrameAsm.at(i) << "\t" 
							<< deltaThorAsm << " \t"
							<< (double) (Statistic.TimeStampThor.at(i)*6.66) << " \t"
							<< (double) (Statistic.TimestampAsm.at(i)*6.66) << " \t";
							
				if (i == 0) {
				fileTstp << "0\t0\t0\t0\t0\t0\t0" ;
					
				//	fileTstp	<< setprecision(10) << (double)Statistic.TimestampAsm.at(0)*6.66 << "ns\t" << (double)Statistic.TimeStampThor.at(0)*4.16 << "ns\t"//*4.16/1000000
				//				<< "\t\t" << Statistic.TimeStamp_Thor_Asm.at(0)*6.66<< "ns \t";
				}
				else {
					double DiffTimeThorAsm = (Statistic.TimestampAsm.at(i)*6.66 - Statistic.TimeStampThor.at(i)*6.66)/1000;
					double DiffTimeAsm  = (((double) Statistic.TimestampAsm.at(i) - (double) Statistic.TimestampAsm.at(i-1))*6.66/1000000);
					double DiffTimeThor = (Statistic.TimeStampThor.at(i) - Statistic.TimeStampThor.at(i-1))*6.66/1000000;
//					double DiffAsmFrameTrig = ((double)Statistic.TimeStamp_Thor_Asm.at(i) - (double)Statistic.TimestampAsm.at(i))*6.66/1000000;
					fileTstp	<< (double)Statistic.TimeStamp_Thor_Asm.at(i)*6.66  << " \t\t"
								<< Statistic.CptTriggerAsm.at(i) <<"\t\t" << Statistic.CptTriggerThor.at(i) << "\t\t"
								<< DiffTimeThorAsm << "\t\t"
								<< DiffTimeThor 	<< "\t\t"
								<< DiffTimeAsm 	<< "\t\t"
								<<(DiffTimeAsm - DiffTimeThor)*1000000 << " \t";
								

					}
					fileTstp	<< endl;
			}
			fileTstp.close();
		}

	}
	
	if (p->Histo) {
		for (int ch=0;ch<24;++ch) {
			string Name0 = "/datas/" + p->DirName + "/HistoCh" + to_string(ch);
			HistoFile[ch].open (Name0.c_str(),ios::out);
			if (HistoFile[ch].is_open()) 
				for (int k=0;k<4096;++k)
					HistoFile[ch] << Histo[ch][k] << endl;
		}
		for (int ch=0;ch<24;++ch) HistoFile[ch].close();
	}
	
	
	cout << endl;
	unsigned long NbTotalframe = Statistic.NbFrameRecorded  + Statistic.LostFrameAsm;
	float longpur = ((float) (Statistic.LongFrame) / (float) NbTotalframe)*100.0;
	float shortpur = ((float) (Statistic.ShortFrame) / (float) NbTotalframe)*100.0;
	float ErrFeIdpur = ((float) (Statistic.ErrFrontEnd) / (float) NbTotalframe)*100.0;
	float LostFrameAsmpur = ((float) (Statistic.LostFrameAsm) / (float) NbTotalframe)*100.0;
	
	
	stringstream Rapport;
	Rapport <<  "***********************************************************************" << endl;
	Rapport << "Analyse run \t" << p->DirName << "\t FeId = 0x" << hex << FeId << dec << endl;
	Rapport << fixed << setprecision(2);
	Rapport << "Frame total Recorded\t\t:\t" << Statistic.NbFrameRecorded << endl;
	if (Statistic.LongFrame >0)		Rapport << "Short Frame total \t\t:\t" << Statistic.ShortFrame  << "\t" << shortpur <<" %\n";
	if (Statistic.LongFrame >0) 		Rapport << "Long Frame total \t\t:\t" 	<< Statistic.LongFrame   << "\t" << longpur  << " %\n";
	if (Statistic.ErrHeader >0) 		Rapport << "Error Header \t\t\t:\t" << Statistic.ErrHeader   << "\n";
	if (Statistic.ErrFrontEnd >0) 	Rapport << "Front Id Error \t\t\t:\t" << Statistic.ErrFrontEnd << "\t" << ErrFeIdpur << " %\n";
	if (Statistic.ShortFragment>0) 	Rapport << "Short Fragment\t\t\t:\t"	<< Statistic.ShortFragment << "\n";
	if (Statistic.LongFragment>0) 	Rapport << "Long Fragment\t\t\t:\t"	 << Statistic.LongFragment << "\n";
	Rapport << "Lost Frame Asm \t\t\t:\t" << Statistic.LostFrameAsm << "\t" << LostFrameAsmpur << "%\t" 
			  << (((float) (Statistic.LostFrameAsm) / (float) Statistic.NbFrameRecorded))*100.0 << "%\n";
	Rapport << "Frame Total Recorded + lost\t:\t" << Statistic.NbFrameRecorded + Statistic.LostFrameAsm << "\n";
	Rapport << "Last Trigger Thor \t\t:\t" << Statistic.CptTriggerThor.back() << "\n";
	if (Statistic.NoTriggerReceived >0) Rapport << "no received Trigger\t\t:\t" << Statistic.NoTriggerReceived << "\n";
	Rapport <<  "***********************************************************************\n";
	
	string NameRapport = p->DirName + "/Rapport" + p->FeId + ".txt";
	cout <<"Create Rapport file " << NameRapport << endl;
	ofstream fileRapport (NameRapport,std::ofstream::app);
	if (fileRapport.is_open()){
		fileRapport << Rapport.str();
		fileRapport.close();
	}
	else cout << "Error opening " << NameRapport << endl;
	
	if (verbose) {
		cout << FgColor::yellow << "***********************************************************************" << FgColor::white << endl;
		cout << Rapport.str() << endl << endl;
		cout << FgColor::yellow << "***********************************************************************" << FgColor::white << endl;
	}
}

int main(int argc, char * argv[])
{
	int cnt = 0;
//	u32 utmp;
	bool fileonly=false;
	string FileOnly;
	struct param Param;
	
	Param.WriteTimesTamp = false;
	Param.DecodeChannel  = false;
	oneFrameLost = 0 ;
	static struct option long_options[] = {
		{"run"				, required_argument , nullptr, 'r' },		// Ok
		{"frontend"		 	, required_argument , nullptr, 'i' },		// Ok
		{"timestamp"		, no_argument       , NULL, 't' }, 
		{"decodechannel"	, no_argument       , NULL, 'd' },
		{"histo"				, no_argument       , NULL, 'g' },
		{"lostframe"		, no_argument       , NULL, 'l' },
		{"file"				, required_argument , NULL, 'f' },
		{"verbose"			, no_argument       , NULL, 'y' },		// Ok
		{"veryverbose"		, no_argument       , NULL, 'z' },		// Ok		
		{NULL					, no_argument       , NULL, 0   }
	};
	srand(time(NULL));
	cout << "Build at " << __TIME__ << " the " << __DATE__ << endl;
	cout << endl
	     << CtColor::underscore << CtColor::reverse << FgColor::yellow
	     << "Analyse File Daq Version " << VERSION << FgColor::white
	     << CtColor::none << endl;

	if(argc<2) {
		cout << "Help ------------->" << endl; 
		cout << "--run <Directory>"  << endl;
		cout << "--frontend <FeID>  \t FeId in Hexa" << endl;
		cout << "--timestamp \t\t" << endl;
	} else {
		int opt;
		while ((opt = getopt_long(argc, argv, "ABCDEFGHIJKLMNOPQRSTUVWXYZ:abcdef:ghijklmopqrstuvwwxyz:", long_options, NULL)) != -1)
		{
			cnt ++;
//			cout << cnt << " arg (" << opt << CFormat(") %c", opt) << endl;
			switch (opt) {
				case '?' :										// Unknow or ambigous option
					return (-1);
				case 'i' :Param.FeId = optarg;Param.feid= stol(Param.FeId,NULL,16);cout << "FrontEnd ID " << Param.FeId  << endl;break;
				case 'r' :Param.DirName = optarg;cout << "Run " << Param.DirName  << endl;break;
				case 't' :Param.WriteTimesTamp = true;break;
				case 'g' :Param.Histo = true;break;
				case 'l' :Param.LostFrame = true;break;
				case 'd' :Param.DecodeChannel  = true;break;
				case 'y' :verbose = true; break;
				case 'f' :FileOnly = optarg;fileonly=true;break;
				
				
			}										// {"all", no_argument, NULL, 'a' },
		}
		if (fileonly) {
			Param.feid = -1;
			Param.FileName = FileOnly;
			cout << Param.FileName << endl;
			ReadFile(&Param);
		}
	   else
		if (Param.feid <= 0x1f) {
		
			Param.FileName = Param.DirName + "/MyFile" + Param.FeId + ".bin";
			cout << Param.FileName << endl;
			ReadFile(&Param);
		}
		else {
			NbframeRec=0;
			string DirName = Param.DirName;
			cout <<DirName << endl;
			if (Param.feid == 0x50) {
				for (short k=0x0;k<0x8;++k) {
					Param.feid = -1;
					std::stringstream sstream;
					sstream  << std::hex << k;
					std::string result = sstream.str();
					Param.FileName = DirName + "/MyFile_eno1_" + result + ".bin";
					cout << Param.FileName << endl;
					ReadFile(&Param);
				}
			}
			else 
			if (Param.feid == 0x60) {  //non tester à ce jour
				for (short k=0x0;k<0x8;++k) {
					Param.feid = -1;
					std::stringstream sstream; 
					sstream << std::hex << k;
					std::string result = sstream.str();
					Param.FileName = Param.DirName + "/MyFile_eno2_" + result + ".bin";
					cout << Param.FileName << endl;
					ReadFile(&Param);
				}
			}
		}
	}
}

