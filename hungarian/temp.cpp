//---------------------------------------------------------------------------
#include "stdafx.h"
//#include <vcl.h>
#include <string>
#include <math.h>
#include <fstream>
#pragma hdrstop
//#include "Main.h"
//---------------------------------------------------------------------------
//#pragma package(smart_init)
//#pragma link "Spin"
//#pragma link "IWImageList"
//#pragma link "cspin"
//#pragma resource "*.dfm"*Form1;MyFName="";Cn,Cm;**C;**C0;**CS;Pm;*P;Zn;*Z;Dn,Dm;**D;OPrz=0;OPtr=0;Xn,Xm;**X;Nn,Nm;**N;posl;pstr;*Sheet;*Grid;*GrX;* LabelC;* LabelX;Fn;*F;Gm;*G;*GridF;L;Lstr;Cep[50][7][8];nomer=0;NKor=0;NIter=0;
//Конструктор формы
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//Вывод подсказки//При создании формы
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	StoimPer->Cells[0][0]="Ai";
	for (int j=1; j<StoimPer->ColCount; j++)
	{
		StoimPer->Cells[j][0]="B"+IntToStr(j);
	}
	for (int i=1;i<StoimPer->RowCount; i++)
	{
		StoimPer->Cells[0][i]="A"+IntToStr(i);
	}
	Potr->Cells[0][0]="Потр";
	Zapas->Cells[0][0]="Запас";
}
//При вводе в ячейку таблицы__fastcall TForm1::TabKeyPress(TObject *Sender, wchar_t &Key)
{
	Set <char, 0, 255> Dig;
	TStringGrid* Tab=(TStringGrid *)Sender;
	Dig<<'0'<<'1'<<'2'<<'3'<<'4'<<'5'<<'6'<<'7'<<'8'<<'9'<<'\b'<<','<<'.'<<VK_RETURN;
	if(!Dig.Contains(Key)){
		Key=0;
		Beep();
	}
}
//При нажатии клавиши "Решить"
void __fastcall TForm1::ReshutClick(TObject *Sender)
{
	nomer=0;
	//Создание матрицы C
	Cn=StoimPer->RowCount-1;
	Cm=StoimPer->ColCount-1;
	for(int i=0;i<50;i++)
		for(int j=0;j<Cn;j++)
			for(int z=0;z<Cm;z++)
				Cep[i][j][z]=0;
	C=new int*[Cn];
	C0=new int*[Cn];
	CS=new int*[Cn];
	for (int i=0; i<Cn; i++){
		C[i]=new int[Cm];
		C0[i]=new int[Cm];
		CS[i]=new int[Cm];
	}
	for(int i=1;i<StoimPer->RowCount;i++)
		for(int j=1;j<StoimPer->ColCount;j++)
		{
			if(StoimPer->Cells[j][i]==""){
				MessageDlg(L"Введите данные в таблицу стоимости перевозки.", mtConfirmation,
					TMsgDlgButtons() << mbOK, 0);
				return;
			}
			else{
				C[i-1][j-1]=(int)(StoimPer->Cells[j][i].ToDouble()*100);
				C0[i-1][j-1]=(int)(StoimPer->Cells[j][i].ToDouble()*100);
			}
		}
		//Создание матрицы потребностей
		Pm=Potr->ColCount-1;
		P=new int[Pm];
		for(int i=1;i<Potr->ColCount;i++)
		{
			if(Potr->Cells[i][0]=="")
			{
				MessageDlg(L"Введите данные в таблицу потребления.", mtConfirmation,
					TMsgDlgButtons() << mbOK, 0);
				return;
			}
			else
				P[i-1]=(int)(Potr->Cells[i][0].ToDouble()*100);
		}
		//Создание матрицы запасов
		Zn=Zapas->RowCount-1;
		Z=new int[Zn];
		for(int i=1;i<Zapas->RowCount;i++)
		{
			if(Zapas->Cells[0][i]=="")
			{
				MessageDlg(L"Введите данные в таблицу запасов.", mtConfirmation,
					TMsgDlgButtons() << mbOK, 0);
				return;
			}
			else
				Z[i-1]=(int)(Zapas->Cells[0][i].ToDouble()*100);
		}
		for(int i=0;i<Ogran->RowCount;i++)
			for(int j=0;j<Ogran->ColCount;j++)
				Ogran->Cells[j][i]="10000";
		Dn=Ogran->RowCount;
		Dm=Ogran->ColCount;
		D=new int*[Dn];
		for (int i=0; i<Dn; i++)
			D[i]=new int[Dm];
		for(int i=0;i<Dn;i++)
			for(int j=0;j<Dm;j++)
				D[i][j]=(int)(Ogran->Cells[j][i].ToDouble()*100);
		//Инициализыция невязок
		OPrz=0;
		OPtr=0;
		for(int i=0;i<Zn;i++)
			OPrz+=Z[i];
		for(int i=0;i<Pm;i++)
			OPtr+=P[i];
		if(OPrz!=OPtr){
			MessageDlg(L"Задача не удавлетворяет условию баланса.", mtConfirmation,
				TMsgDlgButtons() << mbOK, 0);
			return;
		}
		if(PageControl->PageCount>2){
			NIter=0;
			for(int i=PageControl->PageCount-1;i>=2;i--)
				PageControl->Pages[i]->~TTabSheet();
		}
		//Создание С штрих
		for(int j=0;j<Cm;j++)
		{
			int min=C[0][j];
			for(int i=1;i<Cn;i++){
				if(C[i][j]<min)
					min=C[i][j];
			}
			for(int i=0;i<Cn;i++){
				C[i][j]-=min;
				CS[i][j]=C[i][j];
			}
		}
		//Создание C0
		for(int i=0;i<Cn;i++){
			int min=C[i][0];
			for(int j=1;j<Cm;j++)
				if(C[i][j]<min)
					min=C[i][j];
			if (min!=0)
				for(int j=0;j<Cm;j++)
					C[i][j]-=min;
		}
		Predv();
		//Создание матрицы опорного плана
		Xn=Cn;
		Xm=Cm;
		X=new int*[Xn];
		for (int i=0; i<Xn; i++)
			X[i]=new int[Xm];
		for (int j=0; j<Cm; j++){
			for (int i=0; i<Cn; i++){
				if(C[i][j]==0){
					//Если ограничение меньше
					if(D[i][j]<=P[j] && D[i][j]<=Z[i]){
						X[i][j]=D[i][j];
						P[j]-=X[i][j];
						Z[i]-=X[i][j];
					}
					else{
						if(P[j]<Z[i]){
							X[i][j]=P[j];
							P[j]-=X[i][j];
							Z[i]-=X[i][j];
						}
						else{
							X[i][j]=Z[i];
							P[j]-=X[i][j];
							Z[i]-=X[i][j];
						}
					}
				}
				else
					X[i][j]=0;
			}
		}
		//проверка опитимальности
		OPrz=0;
		OPtr=0;
		for(int i=0;i<Zn;i++)
			OPrz+=Z[i];
		for(int i=0;i<Pm;i++)
			OPtr+=P[i];
		if((OPrz+OPtr)==0){
			//Заполнения таблицы издержек
			Izderg->RowCount=Cn;
			Izderg->ColCount=Cm;
			for(int i=0;i<Cn;i++)
				for(int j=0;j<Cm;j++)
					Izderg->Cells[j][i]=FormatFloat("",C[i][j]/100.0);
			//Заполнения таблицы опорного плана
			Plan->RowCount=Xn;
			Plan->ColCount=Xm;
			for(int i=0;i<Xn;i++)
				for(int j=0;j<Xm;j++){
					if(X[i][j]==0)
						Plan->Cells[j][i]="";
					else
						Plan->Cells[j][i]=FormatFloat("",X[i][j]/100.0);
				}
				L=0;
				Lstr="L=";
				for(int i=0;i<Cn;i++)
					for(int j=0;j<Cm;j++)
						if(X[i][j]>0)
						{
							L+=X[i][j]*C0[i][j];
							Lstr+=FormatFloat("",C0[i][j]/100.0)+"*"+FormatFloat("",X[i][j]/100.0)+"+";
						}
						Lstr.Delete(Lstr.Length(),1);
						Lstr+="="+FormatFloat("",L/10000.0);
						Edit2->Text=Lstr;
						MessageDlg(L"План оптимизирован.", mtConfirmation,
							TMsgDlgButtons() << mbOK, 0);
						return;
		}
		//создание матрицы нулей
		Nn=Cn;
		Nm=Cm;
		N=new int*[Nn];
		for (int i=0; i<Nn; i++)
			N[i]=new int[Nm];
		//Начало итерации-----------------------------------------------------------
		while((OPrz+OPtr)!=0)
		{
			posl=-1;
			pstr=-1;
			int v=0;
			int *Q=new int[Cn*Cm];
			//определение нулей
			for(int i=0;i<Cn;i++)
				for(int j=0;j<Cm;j++)
				{
					if(C[i][j]==0)
					{
						if(X[i][j]==D[i][j])
							N[i][j]=2;//полный ноль
						else{
							if(X[i][j]==0)
								N[i][j]=0; //обычный ноль
							else
								N[i][j]=1; //неполный ноль
						}
					}
					else
						N[i][j]=-1; //не ноль
				}
				//массив для выделения строк
				Fn=Zn;
				F=new int[Fn];
				for(int i=0;i<Fn;i++)
					F[i]=0;
				//выделение столбцов
				Gm=Pm;
				G=new int[Gm];
				for(int i=0;i<Gm;i++)
				{
					if(P[i]==0)//Потребление равно нулю
						G[i]=1; //Выделение
					else
						G[i]=0; //нет
				}
				bool kor=false; //необходимость коррекции
				bool np; //необходимостть нового поиска
				while(kor==false)
				{
					np=false;
					for(int j=0;j<Nm;j++)
					{
						if(G[j]==1) //если выделен столбец
							continue;
						else
						{
							for(int i=0;i<Nn;i++)
							{
								if(F[i]==1) //если выделена строка
									continue;
								if(N[i][j]==0 || N[i][j]==1)//если неполный ноль
								{
									N[i][j]=3;//обозначение штрихом
									if(Z[i]==0) //если запас равен нулю
									{
										F[i]=1; //выделение строки
										for(int k=0;k<Cm;k++) //проход по строке
										{
											if(G[k]==0) //если строка невыделена
												continue;
											if(N[i][k]==1 || N[i][k]==2)//если сущ. ноль
											{
												N[i][k]=4; //обозначение звездочкой
												G[k]=0; //снятие выделения
												np=true; //необходимость поиска заново
											}
										}
									}
									else
									{
										kor=true; //необходимость коррекции плана
										Q[v]=Z[i]; //запоминание невязки по строке
										v++; //увеличение количества
										Q[v]=D[i][j]-X[i][j]; //резервы насыщения
										v++;
										posl=j; //запоминание места
										pstr=i; //первого элемента
									}
								}
								if(np==true || kor==true)
									break;
							}
						}
						if(np==true || kor==true)
							break;
					}
					//эквивалентне преобразования
					if(np==false && kor==false)
					{
						bool go=false;
						for(int i=0;i<Cn;i++)
						{
							for(int j=0;j<Cm;j++)
							{
								if((F[i]==1 && G[j]==0) || (F[i]==0 && G[j]==1))
									continue;
								else
								{
									if(F[i]==1 && G[j]==1)
									{
										if(C[i][j]<0)
										{
											go=true;
											break;
										}
									}
									else
									{
										if(C[i][j]>0)
										{
											go=true;
											break;
										}
									}
								}
							}
							if(go==true)
								break;
						}
						if(go==false)
						{
							MessageDlg(L"Задача неразрешима.", mtConfirmation,
								TMsgDlgButtons() << mbOK, 0);
							Edit2->Text="L=0";
							return;
						}
						bool first=true;
						int min=0;
						for(int i=0;i<Cn;i++)
						{
							for(int j=0;j<Cm;j++)
							{
								//если один раз выделена ячейка
								if((F[i]==1 && G[j]==0) || (F[i]==0 && G[j]==1))
									continue;
								else
								{
									//если дважды выделена
									if(F[i]==1 && G[j]==1)
									{
										//если первый раз
										if(first==true)
										{
											if(C[i][j]<0)
											{
												min=abs(C[i][j]);
												first=false;
											}
										}
										else
										{
											if(C[i][j]<0)
											{
												if(abs(C[i][j])<min)
													min=abs(C[i][j]);
											}
										}
									}
									//если невыделена
									if(F[i]==0 && G[j]==0)
									{
										if(first==true)
										{
											if(C[i][j]>0)
											{
												min=C[i][j];
												first=false;
											}
										}
										else
										{
											if(C[i][j]>0)
											{
												if(C[i][j]<min)
													min=C[i][j];
											}
										}
									}
								}
							}
						}
						//вычитание элемента
						for(int i=0;i<Cn;i++)
						{
							for(int j=0;j<Cm;j++)
							{
								//если один раз выделена
								if((F[i]==1 && G[j]==0) || (F[i]==0 && G[j]==1))
									continue;
								else
								{
									//если дважды
									if(G[j]==1 && F[i]==1)
										C[i][j]+=min;
									else
										C[i][j]-=min;
								}
							}
						}
						//новая разметка
						for(int i=0;i<Cn;i++)
						{
							for(int j=0;j<Cm;j++)
							{
								if(C[i][j]==0)
								{
									if(X[i][j]==D[i][j])
										N[i][j]=2;
									else
									{
										if(X[i][j]==0)
											N[i][j]=0;
										else
											N[i][j]=1;
									}
								}
								else
									N[i][j]=-1;
							}
						}
						delete []F;
						F=NULL;
						Ecviv();
						//массив для выделения строк
						Fn=Zn;
						F=new int[Fn];
						for(int i=0;i<Fn;i++)
							F[i]=0;
					}
				}
				NKor=0;
				//составление цепочки
				int* Str=new int[Cn*Cm];
				int* Stol=new int[Cn*Cm];
				int str=0;
				int stol=0;
				Str[str]=pstr;
				Stol[stol]=posl;
				Cep[nomer][pstr][posl]=1; //стрела вправо
				str++;
				stol++;
				Stol[stol]=posl; //для начала цепочки
				stol++;
				int stroka=-1;(1)
				{(int i=0;i<Cn;i++)
				{
					if(N[i][Stol[stol-1]]==4) //если сущ. 0*
					{
						stroka=i;
						Str[str]=i;
						if(Str[str]<Str[str-1]) //если новый 0* выше 0'
						{
							if (Stol[stol-2]<Stol[stol-1])//если старый 0* левее 0'
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=2;//слева вверх
								for(int i=Str[str]+1;i<Str[str-1];i++)
								{
									if(Cep[nomer][i][Stol[stol-1]]==3)
										Cep[nomer][i][Stol[stol-1]]=12; //крест
									elseCep[nomer][i][Stol[stol-1]]=4; //вертикально
								}
							}
							else
							{
								if (Stol[stol-2]>Stol[stol-1])//если старый 0* правее 0'
								{
									Cep[nomer][Str[str-1]][Stol[stol-1]]=5;//справа наверх
									for(int i=Str[str]+1;i<Str[str-1];i++)
									{
										if(Cep[nomer][i][Stol[stol-1]]==3)
											Cep[nomer][i][Stol[stol-1]]=12; //крест
										else
											Cep[nomer][i][Stol[stol-1]]=4; //вертикально
									}
								}
								else //если начало цепочки
								{
									for(int i=Str[str]+1;i<=Str[str-1];i++)
									{
										if(Cep[nomer][i][Stol[stol-1]]==3)
											Cep[nomer][i][Stol[stol-1]]=12; //крест
										else
											Cep[nomer][i][Stol[stol-1]]=4; //вертикально
									}
								}
							}
						}
						else //если новый 0* ниже 0'
						{
							if (Stol[stol-2]<Stol[stol-1])//если старый 0* левее 0'
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=6;//слева вниз
								for(int i=Str[str-1]+1;i<Str[str];i++)
								{
									if(Cep[nomer][i][Stol[stol-1]]==3)
										Cep[nomer][i][Stol[stol-1]]=12; //крест
									else
										Cep[nomer][i][Stol[stol-1]]=4; //вертикально
								}
							}
							{
								if (Stol[stol-2]>Stol[stol-1])//если старый 0* правее 0'
								{
									Cep[nomer][Str[str-1]][Stol[stol-1]]=7;//справа вниз
									for(int i=Str[str-1]+1;i<Str[str];i++)
									{
										if(Cep[nomer][i][Stol[stol-1]]==3)
											Cep[nomer][i][Stol[stol-1]]=12; //крест
										else
											Cep[nomer][i][Stol[stol-1]]=4; //вертикально
									}
								}
								else //если начало цепочки
								{
									for(int i=Str[str-1];i<Str[str];i++)
									{
										if(Cep[nomer][i][Stol[stol-1]]==3)
											[nomer][i][Stol[stol-1]]=12; //крест
										else
											Cep[nomer][i][Stol[stol-1]]=4; //вертикально
									}
								}
							}
						}
						str++;
						Q[v]=X[i][Stol[stol-1]];
						v++;
						break;
					}
				}
				if(stroka==-1)
				{
					Q[v]=P[Stol[stol-1]];
					v++;
					break;
				}
				for(int j=0;j<Cm;j++)
				{
					if(N[Str[str-1]][j]==3)//если сущ. 0'
					{
						Stol[stol]=j;
						if(Stol[stol]<Stol[stol-1])//если новый 0' левее 0*
						{
							if (Str[str-2]<Str[str-1]) //если старый 0' выше 0*
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=2;//сверху влево
								for(int i=Stol[stol]+1;i<Stol[stol-1];i++)
								{
									if(Cep[nomer][Str[str-1]][i]==4)
										Cep[nomer][Str[str-1]][i]=12;//крест
									else
										Cep[nomer][Str[str-1]][i]=3;//горизонталь
								}
								Cep[nomer][Str[str-1]][Stol[stol]]=9;//стрела влево
							}
							else //если старый 0' ниже 0*
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=6;//снизу влево
								for(int i=Stol[stol]+1;i<Stol[stol-1];i++)
								{
									if(Cep[nomer][Str[str-1]][i]==4)
										Cep[nomer][Str[str-1]][i]=12;//крест
									else
										Cep[nomer][Str[str-1]][i]=3;//горизонталь
								}
								Cep[nomer][Str[str-1]][Stol[stol]]=9;
							}
						}
						else //если новый 0' правее 0*
						{
							if (Str[str-2]<Str[str-1])//если старый 0' выше 0*
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=5;//слева вниз
								for(int i=Stol[stol-1]+1;i<Stol[stol];i++)
								{
									if(Cep[nomer][Str[str-1]][i]==4)
										Cep[nomer][Str[str-1]][i]=12;//крест
									else
										Cep[nomer][Str[str-1]][i]=3;//горизонталь
								}
								Cep[nomer][Str[str-1]][Stol[stol]]=1;
							}
							else //если старый 0' ниже 0*
							{
								Cep[nomer][Str[str-1]][Stol[stol-1]]=7;//снизу вправо
								for(int i=Stol[stol-1]+1;i<Stol[stol];i++)
								{
									if(Cep[nomer][Str[str-1]][i]==4)
										Cep[nomer][Str[str-1]][i]=12;//крест
									else
										Cep[nomer][Str[str-1]][i]=3;//горизонталь
								}
								Cep[nomer][Str[str-1]][Stol[stol]]=1;
							}
						}
						stol++;
						Q[v]=D[Str[str-1]][j]-X[Str[str-1]][j];
						v++;
						break;
					}
				}
				stroka=-1;
				}
				nomer++;
				//Создание новой вкладки
				NewTab();
				int min=Q[0];
				for(int i=1;i<v;i++)
				{
					if(Q[i]<min)
						min=Q[i];
				}
				str--;
				stol--;
				X[Str[str]][Stol[stol]]+=min;
				P[Stol[stol]]-=min;
				while(1)
				{
					if(str==0)
					{
						Z[Str[str]]-=min;
						break;
					}
					stol--;
					X[Str[str]][Stol[stol]]-=min;
					str--;
					X[Str[str]][Stol[stol]]+=min;
				}
				OPrz=0;
				OPtr=0;
				for(int i=0;i<Zn;i++)
					OPrz+=Z[i];
				for(int i=0;i<Pm;i++)
					OPtr+=P[i];
				delete []G;
				G=NULL;
				delete []F;
				F=NULL;
				delete []Q;
				Q=NULL;
				delete []Str;
				Str=NULL;
				delete []Stol;
				Stol=NULL;
		}
		//!!!
		MessageDlg(L"Задача оптимизированa.", mtConfirmation,
			TMsgDlgButtons() << mbOK, 0);
		//Заполнения таблицы издержек
		Izderg->RowCount=Cn;
		Izderg->ColCount=Cm;
		for(int i=0;i<Cn;i++)
		{
			for(int j=0;j<Cm;j++)
			{
				switch (N[i][j])
				{
				case 4:
					Izderg->Cells[j][i]="0*";
					break;
				case 3:
					Izderg->Cells[j][i]="0'";
					break;
				case 2:
					Izderg->Cells[j][i]=L'\u022a';
					break;
				case 1:
					Izderg->Cells[j][i]=L'\u0230';
					break;
				case 0:
					Izderg->Cells[j][i]="0";
				case -1:
					Izderg->Cells[j][i]=FormatFloat("",C[i][j]/100.0);
					break;
				}
			}
		}
		//Заполнения таблицы опорного плана
		Plan->RowCount=Xn;
		Plan->ColCount=Xm;
		for(int i=0;i<Xn;i++)
		{
			for(int j=0;j<Xm;j++)
			{
				if(X[i][j]==0)
					Plan->Cells[j][i]="";
				else
					Plan->Cells[j][i]=FormatFloat("",X[i][j]/100.0);
			}
		}
		L=0;
		Lstr="L=";
		for(int i=0;i<Cn;i++)
		{
			for(int j=0;j<Cm;j++)
			{
				if(X[i][j]>0)
				{
					L+=X[i][j]*C0[i][j];
					Lstr+=FormatFloat("",C0[i][j]/100.0)+"*"+FormatFloat("",X[i][j]/100.0)+"+";
				}
			}
		}
		Lstr.Delete(Lstr.Length(),1);
		Lstr+="="+FormatFloat("",L/10000.0);
		Edit2->Text=Lstr;
		//Освобождение памяти
		delete []C;
		C=NULL;
		delete []P;
		P=NULL;
		delete []Z;
		Z=NULL;
		delete []D;
		D=NULL;
		delete []X;
		X=NULL;
		delete []N;
		N=NULL;
}__fastcall TForm1::NOpenClick(TObject *Sender)
{
	if(OpenDialog1->Execute())
	{
		MyFName=OpenDialog1->FileName;
		Open(OpenDialog1->FileName);
	}
}
//---------------------------------------------------------------------------__fastcall TForm1::NSaveAsClick(TObject *Sender)
{
	SaveDialog1->FileName=MyFName;
	if(SaveDialog1->Execute())
	{
		MyFName=SaveDialog1->FileName;
		Save(SaveDialog1->FileName);
	}
}
//---------------------------------------------------------------------------__fastcall TForm1::NSaveClick(TObject *Sender)
{
	if(MyFName!="")
		Save(MyFName);
	else
		if(SaveDialog1->Execute())
		{
			MyFName=SaveDialog1->FileName;
			Save(SaveDialog1->FileName);
		}
}
//---------------------------------------------------------------------------__fastcall TForm1::NNewClick(TObject *Sender)
{
	Zapas->RowCount=8;
	for(int i=1;i<Zapas->RowCount;i++)
	{
		Zapas->Cells[0][i]="";
	}
	StoimPer->ColCount=8;
	StoimPer->RowCount=8;
	for(int i=1;i<StoimPer->RowCount;i++)
	{
		for(int j=1;j<StoimPer->ColCount;j++)
		{
			StoimPer->Cells[j][i]="";
		}
	}
	Ogran->ColCount=8;
	Ogran->RowCount=8;
	Potr->ColCount=8;
	for(int j=1;j<Potr->ColCount;j++)
	{
		Potr->Cells[j][0]="";
	}
	Edit2->Text="L=";
}
//---------------------------------------------------------------------------TForm1::Save(UnicodeString adr)
{
	ofstream f(adr.t_str() ,ios::out);
	for(int i=1;i<Zapas->RowCount;i++)
	{
		if(Zapas->Cells[0][i]=="")
		{
			f<<"0 ";
		}
		else
		{
			f<<Zapas->Cells[0][i].ToDouble()<<" ";
		}
	}
	f<<"\n\n";
	for(int i=1;i<StoimPer->RowCount;i++)
	{
		for(int j=1;j<StoimPer->ColCount;j++)
		{
			if(StoimPer->Cells[j][i]=="")
			{
				f<<"0 ";
			}
			else
			{
				f<<StoimPer->Cells[j][i].ToDouble()<<" ";
			}
		}
		f<<"\n";
	}
	f<<"\n";
	for(int i=0;i<Ogran->RowCount;i++)
	{
		for(int j=0;j<Ogran->ColCount;j++)
		{
			if(Ogran->Cells[j][i]=="")
			{
				f<<"0 ";
			}
			else
			{
				f<<Ogran->Cells[j][i].ToDouble()<<" ";
			}
		}
		f<<"\n";
	}
	f<<"\n";
	for(int i=1;i<Potr->ColCount;i++)
	{
		if(Potr->Cells[i][0]=="")
		{
			f<<"0 ";
		}
		else
		{
			f<<Potr->Cells[i][0].ToDouble()<<" ";
		}
	}
	f.close();
}
//------------------------------------------------------------------------TForm1::Open(UnicodeString adr)
{
	int k;
	double d;
	ifstream f(adr.t_str() ,ios::in|ios::nocreate);
	f>>k;
	//UDStrok->Position=k;
	f>>k;
	//UDStolb->Position=k;
	//Zapas->RowCount=UDStrok->Position+1;
	for(int i=1;i<Zapas->RowCount;i++)
	{
		f>>d;
		Zapas->Cells[0][i]=FormatFloat("",d);
	}
	//StoimPer->RowCount=UDStrok->Position+1;
	//StoimPer->ColCount=UDStolb->Position+1;
	for(int i=1;i<StoimPer->RowCount;i++)
	{
		for(int j=1;j<StoimPer->ColCount;j++)
		{
			f>>d;
			StoimPer->Cells[j][i]=FormatFloat("",d);
		}
	}
	//Ogran->RowCount=UDStrok->Position;
	//Ogran->ColCount=UDStolb->Position;
	for(int i=0;i<Ogran->RowCount;i++)
	{
		for(int j=0;j<Ogran->ColCount;j++)
		{
			f>>d;
			Ogran->Cells[j][i]=FormatFloat("",d);
		}
	}
	//Potr->ColCount=UDStolb->Position+1;
	for(int i=1;i<Potr->ColCount;i++)
	{
		f>>d;
		Potr->Cells[i][0]=FormatFloat("",d);
	}
	f.close();
	Edit2->Text="L=";
}TForm1::NewTab()
{
	Sheet=new TTabSheet(this);
	Grid=new TStringGrid(this);
	GrX=new TStringGrid(this);
	LabelC=new TLabel(this);
	LabelX=new TLabel(this);
	Sheet->Parent=this;
	Sheet->PageControl=PageControl;
	NIter++;
	Sheet->Caption="Итер. №"+IntToStr(NIter);
	Sheet->Name="S"+IntToStr(NIter-1);
	Grid->Parent=Sheet;
	Grid->OnDrawCell=StoimPer->OnDrawCell;
	Grid->Left=29;
	Grid->Top=34;
	Grid->Height=181;
	Grid->Width=256;
	Grid->DefaultColWidth=35;
	Grid->DefaultRowHeight=24;
	Grid->FixedCols=0;
	Grid->FixedRows=0;
	Grid->ScrollBars=ssNone;
	Grid->RowCount=Cn;
	Grid->ColCount=Cm;
	for(int i=0;i<Cn;i++)
	{
		for(int j=0;j<Cm;j++)
		{
			switch (N[i][j])
			{
			case 4:
				Grid->Cells[j][i]="0*";
				break;
			case 3:
				Grid->Cells[j][i]="0'";
				break;
			case 2:
				Grid->Cells[j][i]=L'\u022a';
				break;
			case 1:
				Grid->Cells[j][i]=L'\u0230';
				break;
			case 0:
				Grid->Cells[j][i]="0";
			case -1:
				Grid->Cells[j][i]=FormatFloat("",C[i][j]/100.0);
				break;
			}
		}
	}
	LabelC->Parent=Sheet;
	LabelC->Left=17;
	LabelC->Top=18;
	LabelC->Caption="C"+IntToStr(NIter);
	GrX->Parent=Sheet;
	GrX->Left=29;
	GrX->Top=221;
	GrX->Height=181;
	GrX->Width=256;
	GrX->DefaultColWidth=35;
	GrX->DefaultRowHeight=24;
	GrX->FixedCols=0;
	GrX->FixedRows=0;
	GrX->ScrollBars=ssNone;
	GrX->RowCount=Cn;
	GrX->ColCount=Cm;
	for(int i=0;i<Cn;i++)
		for(int j=0;j<Cm;j++)
			if(X[i][j]!=0)
				GrX->Cells[j][i]=FormatFloat("",X[i][j]/100.0);
	LabelX->Parent=Sheet;
	LabelX->Left=17;
	LabelX->Top=200;
	LabelX->Caption="X"+IntToStr(NIter);
	TStringGrid *GridP=new TStringGrid(this);
	GridP->Parent=Sheet;
	GridP->Left=286;
	GridP->Top=221;
	GridP->Height=181;
	GridP->Width=39;
	GridP->ColCount=1;
	GridP->DefaultColWidth=35;
	GridP->DefaultRowHeight=24;
	GridP->FixedCols=0;
	GridP->FixedRows=0;
	GridP->RowCount=Cn;
	GridP->ScrollBars=ssNone;
	for(int i=0;i<Cn;i++)
		GridP->Cells[0][i]=FormatFloat("",Z[i]/100.0);
	TStringGrid* GridB=new TStringGrid(this);
	GridB->Parent=Sheet;
	GridB->Left=30;
	GridB->Top=405
		;
	GridB->Height=25;
	GridB->Width=256;
	GridB->ColCount=Cm;
	GridB->DefaultColWidth=35;
	GridB->DefaultRowHeight=24;
	GridB->FixedCols=0;
	GridB->FixedRows=0;
	GridB->RowCount=1;
	GridB->ScrollBars=ssNone;
	for(int i=0;i<Cm;i++)
		GridB->Cells[i][0]=FormatFloat("",P[i]/100.0);
	GridF=new TStringGrid(this);
	GridF->Parent=Sheet;
	GridF->Left=286;
	GridF->Top=34;
	GridF->Height=181;
	GridF->Width=16;
	GridF->ColCount=1;
	GridF->DefaultColWidth=12;
	GridF->DefaultRowHeight=24;
	GridF->FixedCols=0;
	GridF->FixedRows=0;
	GridF->RowCount=Cn;
	GridF->ScrollBars=ssNone;
	for(int i=0;i<Cn;i++)
		if(F[i]==1)
			GridF->Cells[0][i]="+";
	TStringGrid *GridG=new TStringGrid(this);
	GridG->Parent=Sheet;
	GridG->Left=30;
	GridG->Top=13;
	GridG->Height=16;
	GridG->Width=256;
	GridG->ColCount=Cm;
	GridG->DefaultColWidth=35;
	GridG->DefaultRowHeight=12;
	GridG->FixedCols=0;
	GridG->FixedRows=0;
	GridG->RowCount=1;
	GridG->ScrollBars=ssNone;
	for(int i=0;i<Cm;i++)
		if(G[i]==1)
			GridG->Cells[i][0]="+";
}TForm1::Predv()
{
	Sheet=new TTabSheet(this);
	Grid=new TStringGrid(this);
	GrX=new TStringGrid(this);
	LabelC=new TLabel(this);
	LabelX=new TLabel(this);
	Sheet->Parent=this;
	Sheet->PageControl=PageControl;
	Sheet->Caption="Предв. этап";
	Grid->Parent=Sheet;
	Grid->Left=30;
	Grid->Top=39;
	Grid->Height=181;
	Grid->Width=256;
	Grid->DefaultColWidth=35;
	Grid->DefaultRowHeight=24;
	Grid->FixedCols=0;
	Grid->FixedRows=0;
	Grid->ScrollBars=ssNone;
	Grid->RowCount=Cn;
	Grid->ColCount=Cm;
	for(int i=0;i<Cn;i++)
	{
		for(int j=0;j<Cm;j++)
		{
			Grid->Cells[j][i]=FormatFloat("",CS[i][j]/100.0);
		}
	}
	LabelC->Parent=Sheet;
	LabelC->Left=3;
	LabelC->Top=20;
	LabelC->Caption="C'";
	GrX->Parent=Sheet;
	GrX->Left=30;
	GrX->Top=239;
	GrX->Height=181;
	GrX->Width=256;
	GrX->DefaultColWidth=35;
	GrX->DefaultRowHeight=24;
	GrX->FixedCols=0;
	GrX->FixedRows=0;
	GrX->ScrollBars=ssNone;
	GrX->RowCount=Cn;
	GrX->ColCount=Cm;
	for(int i=0;i<Cn;i++)
		for(int j=0;j<Cm;j++)
			GrX->Cells[j][i]=FormatFloat("",C[i][j]/100.0);
	LabelX->Parent=Sheet;
	LabelX->Left=3;
	LabelX->Top=223;
	LabelX->Caption="C0";
}TForm1::Ecviv()
{
	Sheet=new TTabSheet(this);
	Grid=new TStringGrid(this);
	LabelC=new TLabel(this);
	Sheet->Parent=this;
	Sheet->PageControl=PageControl;
	NKor++;
	Sheet->Caption="Экв. пр. №"+IntToStr(NIter+1)+"."+IntToStr(NKor);
	Grid->Parent=Sheet;
	Grid->Left=30;
	Grid->Top=39;
	Grid->Height=181;
	Grid->Width=256;
	Grid->DefaultColWidth=35;
	Grid->DefaultRowHeight=24;
	Grid->FixedCols=0;
	Grid->FixedRows=0;
	Grid->ScrollBars=ssNone;
	Grid->RowCount=Cn;
	Grid->ColCount=Cm;
	for(int i=0;i<Cn;i++)
	{
		for(int j=0;j<Cm;j++)
		{
			switch (N[i][j])
			{
			case 4:
				Grid->Cells[j][i]="0*";
				break;
			case 3:
				Grid->Cells[j][i]="0'";
				break;
			case 2:
				Grid->Cells[j][i]=L'\u022a';
				break;
			case 1:
				Grid->Cells[j][i]=L'\u0230';
				break;
			case 0:
				Grid->Cells[j][i]="0";
			case -1:
				Grid->Cells[j][i]=FormatFloat("",C[i][j]/100.0);
				break;
			}
		}
	}
	LabelC->Parent=Sheet;
	LabelC->Left=3;
	LabelC->Top=20;
	LabelC->Caption="C"+IntToStr(NKor);
}__fastcall TForm1::CepochDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
	TGridDrawState State)
{
	TStringGrid* c=(TStringGrid*) Sender;
	if (c->Name!="StoimPer")
	{
		UnicodeString Index;
		Index=PageControl->ActivePage->Name;
		Index.Delete(1,1);
		nomer=Index.ToInt();
		Graphics::TBitmap *pict = new Graphics::TBitmap();
		switch (Cep[nomer][ARow][ACol])
		{
		case 12:
			pict->LoadFromResourceID((unsigned int)HInstance,104);
			break;
		case 9:
			pict->LoadFromResourceID((unsigned int)HInstance,102);
			break;
		case 7:
			pict->LoadFromResourceID((unsigned int)HInstance,106);
			break;
		case 6:
			pict->LoadFromResourceID((unsigned int)HInstance,105);
			break;
		case 5:
			pict->LoadFromResourceID((unsigned int)HInstance,108);
			break;
		case 4:
			pict->LoadFromResourceID((unsigned int)HInstance,109);
			break;
		case 3:
			pict->LoadFromResourceID((unsigned int)HInstance,101);
			break;
		case 2:
			pict->LoadFromResourceID((unsigned int)HInstance,107);
			break;
		case 1:
			pict->LoadFromResourceID((unsigned int)HInstance,103);
			break;
		}
		pict->Transparent = true;
		c->Canvas->Draw(Rect.left,Rect.Top,pict);
		pict->Free();
	}
	nomer=0;
}
//---------------------------------------------------------------------------
//!!__fastcall TForm1::Timer1Timer(TObject *Sender)
{(StoimPer->ColCount == 8)
	Button1->Enabled = false;Button1->Enabled = true;(StoimPer->ColCount == 4)
	Button2->Enabled = false;Button2->Enabled = true;(StoimPer->RowCount == 8)
	Button3->Enabled = false;Button3->Enabled = true;(StoimPer->RowCount == 4)
	Button4->Enabled = false;Button4->Enabled = true;
}
//---------------------------------------------------------------------------__fastcall TForm1::Button1Click(TObject *Sender)
{>ColCount++;>ColCount++;>ColCount++;
}
//---------------------------------------------------------------------------__fastcall TForm1::Button2Click(TObject *Sender)
{>ColCount--;>ColCount--;>ColCount--;
}
//---------------------------------------------------------------------------__fastcall TForm1::Button3Click(TObject *Sender)
{>RowCount++;>RowCount++;>RowCount++;
}
//---------------------------------------------------------------------------__fastcall TForm1::Button4Click(TObject *Sender)
{>RowCount--;>RowCount--;>RowCount--;
}
//---------------------------------------------------------------------------__fastcall TForm1::NAboutClick(TObject *Sender)
{(L"Костенко И-23 Венгерский метод.", mtConfirmation,
	TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------