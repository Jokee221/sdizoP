#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <math.h>
#include <string>
#include <iomanip>
#include <queue>
#include <vector>
#include <set>

using namespace std;
bool * odwiedzone;
double PCFreq = 0.0;
__int64 licznik = 0;
void czasStart()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "B��d!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	licznik = li.QuadPart;
}
void pobierzCzas()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	cout << endl;
	cout << "Czas trwania algorytmu wynosi: " << (li.QuadPart - licznik) / PCFreq << " ms" << endl;
	licznik = 0;
}
class Krawedz
{
public: int wp, wk, waga;
		bool operator () (const Krawedz & k1, const Krawedz & k2)
		{
			if (k1.waga > k2.waga) return true;
			if (k1.waga < k2.waga) return false;
			return false;
		}
};
class ElementListy
{
public:
	ElementListy * nastepny;
	int w, waga;
};
class Stos
{
private:
	ElementListy * s;
public:
	Stos()
	{
		s = NULL;
	}
	~Stos()
	{
		while (s) pop();
	}
	bool empty()
	{
		return !s;
	}
	int top()
	{
		return s->w;
	}
	void pop()
	{
		if (s)
		{
			ElementListy * e = s;
			s = s->nastepny;
			delete e;
		}
	}
	void push(int w)
	{
		ElementListy * e = new ElementListy;
		e->w = w;
		e->nastepny = s;
		s = e;
	}
};
class DrzewoSpinajace
{
private:
	ElementListy ** T;
	Krawedz *kk;
	int rozmiarT;
	int waga, krawedzie;
	int a;

public:
	DrzewoSpinajace(int wierzcholki, int kr)
	{
		T = new ElementListy *[wierzcholki];
		for (int i = 0; i < wierzcholki; i++)
		{
			T[i] = NULL;
		}
		rozmiarT = wierzcholki - 1;
		waga = 0;
		krawedzie = kr;
		kk = new Krawedz[krawedzie];
		a = 0;
	}

	~DrzewoSpinajace()
	{
		ElementListy *e1, *e2;
		for (int i = 0; i <= rozmiarT; i++)
		{
			e1 = T[i];
			while (e1)
			{
				e2 = e1;
				e1 = e1->nastepny;
				delete e2;
			}
		}

		delete[]T;
	}


	void dodajKrawedz(Krawedz k)
	{
		ElementListy *e;

		waga += k.waga;
		e = new ElementListy;
		e->w = k.wk;
		e->waga = k.waga;
		e->nastepny = T[k.wp];
		T[k.wp] = e;

		e = new ElementListy;
		e->w = k.wp;
		e->waga = k.waga;
		e->nastepny = T[k.wk];
		T[k.wk] = e;

		kk[a] = k;
		a++;
	}

	void wyswietl()
	{
		cout << endl;
		for (int i = 0; i < a; i++)
		{

			cout << "Krawedz " << i + 1 << ": ";

			cout << kk[i].wp << "<-->" << kk[i].wk << " [" << kk[i].waga << "] ";

			cout << endl;
		}
		cout << endl << endl << "Waga minimalnego drzewa rozpinajacego = " << waga << endl << endl;
	}

};
class Graf
{
public:
	Krawedz * K, *KO;
	int ** macierzIncydencji, ** grafNieskierowanyM;
	ElementListy ** listySasiedztwa, ** grafNieskierowanyL;

private:
	int wierzcholki, krawedzie, gestosc, krawedzieOdwrotne;
	ElementListy *e1, *e2;
public:
	Graf(int w, int m)
	{
		krawedzie = m;
		wierzcholki = w;
		macierzIncydencji = new int *[wierzcholki];
		grafNieskierowanyM = new int *[wierzcholki];
		listySasiedztwa = new ElementListy *[wierzcholki];
		grafNieskierowanyL = new ElementListy *[wierzcholki];
		K = new Krawedz[krawedzie];
		KO = new Krawedz[krawedzie];
		krawedzieOdwrotne = 0;
	}
	~Graf()
	{
		int i;
		for (int i = 0; i < wierzcholki; i++)
		{
			e1 = listySasiedztwa[i];
			while (e1)
			{
				e2 = e1;
				e1 = e1->nastepny;
				delete e2;
			}
		}
		delete[] listySasiedztwa;


		for (i = 0; i < wierzcholki; i++) delete[] macierzIncydencji[i];
		delete[] macierzIncydencji;


	}
	void grafNieskierowany()
	{
		int i, j;
		for (i = 0; i < wierzcholki; i++)
			grafNieskierowanyM[i] = new int[krawedzie];
		for (i = 0; i < wierzcholki; i++)
			for (j = 0; j < krawedzie; j++)
			{
				grafNieskierowanyM[i][j] = 0;
			}

		for (i = 0; i < wierzcholki; i++)
			grafNieskierowanyL[i] = NULL;

		for (i = 0; i < krawedzie; i++)
		{
			j = 0;
			while (j < krawedzieOdwrotne)
			{

				if (KO[j].wp == K[i].wp && KO[j].wk == K[i].wk)
				{
					break;
				}
				else j++;
			}
			if (j == krawedzieOdwrotne)
			{
				KO[krawedzieOdwrotne].wp = K[i].wk;
				KO[krawedzieOdwrotne].wk = K[i].wp;
				KO[krawedzieOdwrotne].waga = K[i].waga;
				krawedzieOdwrotne++;
			}
		}

		int wp, wk, waga;
		for (int i = 0; i < krawedzieOdwrotne; i++)
		{
			wp = KO[i].wp;
			wk = KO[i].wk;
			waga = KO[i].waga;
			e1 = new ElementListy;
			e1->w = wk;
			e1->waga = waga;
			e1->nastepny = grafNieskierowanyL[wp];
			grafNieskierowanyL[wp] = e1;
			e2 = new ElementListy;
			wp = KO[i].wk;
			wk = KO[i].wp;
			waga = KO[i].waga;
			e2->w = wk;
			e2->waga = waga;
			e2->nastepny = grafNieskierowanyL[wp];
			grafNieskierowanyL[wp] = e2;

			grafNieskierowanyM[KO[i].wp][i] = 1;
			grafNieskierowanyM[KO[i].wk][i] = 1;
		}

	}
	void losujKrawedzie()
	{
		int c = 0;
		int * T;
		int b, doDodania1, doDodania2, a, i, j;
		int liczStopien = 0;
		int * wDrzewie;
		int * nieWDrzewie;
		a = wierzcholki;
		nieWDrzewie = new int[a];
		for (i = 0; i < a; i++)
		{
			nieWDrzewie[i] = i;
		}
		wDrzewie = new int[a];
		b = rand() % a;
		doDodania1 = nieWDrzewie[b];
		nieWDrzewie[b] = nieWDrzewie[a - 1];
		a--;
		wDrzewie[c] = doDodania1;
		c++;

		for (i = 0; i < (wierzcholki - 1); i++)
		{
			b = rand() % c;
			doDodania1 = wDrzewie[b];

			b = rand() % a;
			doDodania2 = nieWDrzewie[b];
			nieWDrzewie[b] = nieWDrzewie[a - 1];
			a--;
			wDrzewie[c] = doDodania2;
			c++;
			K[i].wp = doDodania1;
			K[i].wk = doDodania2;
		}
		for (i = 0; i < (wierzcholki - 1); i++)
		{

			b = rand() % wierzcholki;
			swap(K[b].wk, K[b].wp);

		}

		for (i = wierzcholki - 1; i < krawedzie; i++)
		{
			a = wierzcholki;
			T = new int[a];
			for (int k = 0; k < a; k++)
			{
				T[k] = k;
			}

			b = rand() % a;
			doDodania1 = T[b];
			T[b] = T[a - 1];
			a--;
			while (true)
			{

				liczStopien = 0;
				for (j = 0; j < i; j++)
				{
					if (K[j].wp == doDodania1)
						liczStopien++;
					if (liczStopien == wierzcholki - 1) break;
				}

				if (liczStopien == wierzcholki - 1)
				{
					b = rand() % a;
					doDodania1 = T[b];
					T[b] = T[a - 1];
					a--;
				}

				else break;
			}

			a = wierzcholki;
			for (int k = 0; k < a; k++)
			{
				T[k] = k;
			}

			T[doDodania1] = T[a - 1];
			a--;

			b = rand() % a;
			doDodania2 = T[b];
			T[b] = T[a - 1];
			a--;

			for (j = 0; j < i; j++)
			{
				while (doDodania1 == K[j].wp && doDodania2 == K[j].wk)
				{
					b = rand() % a;
					doDodania2 = T[b];
					T[b] = T[a - 1];
					a--;
					j = 0;
				}
			}
			K[i].wp = doDodania1;
			K[i].wk = doDodania2;
			delete[]T;
		}

		for (i = 0; i < krawedzie; i++)
			K[i].waga = (rand() % 9) + 1;

		delete[] nieWDrzewie;
		delete[] wDrzewie;
	}
	void losowyGraf()
	{
		int i, j;
		for (i = 0; i < wierzcholki; i++)
			macierzIncydencji[i] = new int[krawedzie];
		for (i = 0; i < wierzcholki; i++)
			for (j = 0; j < krawedzie; j++)
			{
				macierzIncydencji[i][j] = 0;
			}

		for (i = 0; i < wierzcholki; i++)
			listySasiedztwa[i] = NULL;

		losujKrawedzie();
		grafNieskierowany();
		for (i = 0; i < krawedzie; i++)
		{
			int wp = K[i].wp;
			int wk = K[i].wk;
			int waga = K[i].waga;
			e1 = new ElementListy;
			e1->w = wk;
			e1->waga = waga;
			e1->nastepny = listySasiedztwa[wp];
			listySasiedztwa[wp] = e1;

			macierzIncydencji[wp][i] = 1;
			macierzIncydencji[wk][i] = -1;
		}

	}
	int getWierzcholki()
	{
		return wierzcholki;
	}
	int getKrawedzie()
	{
		return krawedzie;
	}
	bool czySpojny()
	{
		Stos stos;
		int w, u, i;
		int licz = 0;

		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}

		stos.push(0);
		odwiedzone[0] = true;

		while (!stos.empty())
		{
			w = stos.top();
			stos.pop();
			licz++;
			for (e1 = grafNieskierowanyL[w]; e1; e1 = e1->nastepny)
			{
				u = e1->w;
				if (!odwiedzone[u])
				{
					odwiedzone[u] = true;
					stos.push(u);
				}
			}
		}
		delete[]odwiedzone;
		if (licz == wierzcholki)
			return true;

		else return false;
	}
	
	void DijkstraLista(int w, int t)
	{
		int korzen, wezel, rozmiarKopca, ojciec, lewySyn, prawySyn, kosztMin, synMin, syn, *koszta, *poprzednicy, *kopiec, *pozycjaKopiec;
		Stos stos;
		int szerokosc, i, j;
		koszta = new int[wierzcholki];
		poprzednicy = new int[wierzcholki];
		odwiedzone = new bool[wierzcholki];
		kopiec = new int[wierzcholki];
		pozycjaKopiec = new int[wierzcholki];

		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}
		cout << endl << "Graf skierowany poczatkowy: " << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << "LS[" << i << "] =";
			e1 = listySasiedztwa[i];
			while (e1)
			{
				cout << setw(3) << e1->w << "(" << e1->waga << ") ";
				e1 = e1->nastepny;
			}
			cout << endl;
		}
		cout << endl;

		for (i = 0; i < wierzcholki; i++)
		{
			koszta[i] = MAXINT;
			poprzednicy[i] = -1;
			odwiedzone[i] = false;
			kopiec[i] = pozycjaKopiec[i] = i;
		}
		czasStart();
		rozmiarKopca = wierzcholki;

		koszta[w] = 0;
		wezel = kopiec[0];
		kopiec[0] = kopiec[w];
		kopiec[w] = wezel;
		pozycjaKopiec[w] = 0;
		pozycjaKopiec[0] = w;

		for (i = 0; i < wierzcholki; i++)
		{
			korzen = kopiec[0];

			kopiec[0] = kopiec[--rozmiarKopca];
			pozycjaKopiec[kopiec[0]] = ojciec = 0;
			while (true)
			{
				lewySyn = ojciec + ojciec + 1;
				prawySyn = lewySyn + 1;
				if (lewySyn >= rozmiarKopca) break;
				kosztMin = koszta[kopiec[lewySyn]];
				synMin = lewySyn;
				if ((prawySyn < rozmiarKopca) && (kosztMin > koszta[kopiec[prawySyn]]))
				{
					kosztMin = koszta[kopiec[prawySyn]];
					synMin = prawySyn;
				}
				if (koszta[kopiec[ojciec]] <= kosztMin)
					break;
				wezel = kopiec[ojciec];
				kopiec[ojciec] = kopiec[synMin];
				kopiec[synMin] = wezel;
				pozycjaKopiec[kopiec[ojciec]] = ojciec;
				pozycjaKopiec[kopiec[synMin]] = synMin;
				ojciec = synMin;
			}

			odwiedzone[korzen] = true;

			if (listySasiedztwa[korzen] != NULL)
				for (e1 = listySasiedztwa[korzen]; e1; e1 = e1->nastepny)
					if (!odwiedzone[e1->w] && (koszta[e1->w] > koszta[korzen] + e1->waga))
					{
						koszta[e1->w] = koszta[korzen] + e1->waga;
						poprzednicy[e1->w] = korzen;

						for (syn = pozycjaKopiec[e1->w]; syn; syn = ojciec)
						{
							ojciec = syn / 2;
							if (koszta[kopiec[ojciec]] <= koszta[kopiec[syn]])
								break;
							wezel = kopiec[ojciec];
							kopiec[ojciec] = kopiec[syn];
							kopiec[syn] = wezel;
							pozycjaKopiec[kopiec[ojciec]] = ojciec;
							pozycjaKopiec[kopiec[syn]] = syn;
						}
					}
		}
		pobierzCzas();
		cout << endl;

		cout << "Najkrotsza droga z wierzcholka nr " << w << " do wierzcholka nr: " << t << endl;
		string displayText = "";
		string temp = "";
		int minCost = MAXINT;

		for (i = 0; i < wierzcholki; i++)
		{
			szerokosc = -2;
						
			if (koszta[t] == MAXINT || koszta[t] < 0)
			{
				cout << "Brak sciezki" << endl;
				break;
			}
			else

			{
				for (j = i; j > -1; j = poprzednicy[j])
				{
					stos.push(j);
					szerokosc = szerokosc + 2;
				}

				int koncowyWierzcholek;
				while (!stos.empty())
				{
					koncowyWierzcholek = stos.top();
					temp += to_string(stos.top()) + " ";
					stos.pop();
				}
				//for (j = 0; j < wierzcholki - szerokosc; j++)
					temp += " waga sciezki";
				temp += "(" + to_string(koszta[i]);
				temp += ")";

				if (koncowyWierzcholek == t) {
					displayText = temp;
				}
				temp = "";
			}
		}

		cout << displayText;
		cout << endl << endl;
	}
	void DijkstraMacierz(int w, int t)
	{
		int korzen, wezel, rozmiarKopca, ojciec, lewySyn, prawySyn, kosztMin, synMin, syn, *koszta, *poprzednicy, *kopiec, *pozycjaKopiec;
		Stos stos;
		int szerokosc, i, j, l;
		koszta = new int[wierzcholki];
		poprzednicy = new int[wierzcholki];
		odwiedzone = new bool[wierzcholki];
		kopiec = new int[wierzcholki];
		pozycjaKopiec = new int[wierzcholki];

		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}

		cout << "Graf skierowany poczatkowy: " << endl << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << i;

		}
		cout << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << setw(3) << i;
			for (int j = 0; j < krawedzieOdwrotne; j++)
				cout << setw(3) << macierzIncydencji[i][j];
			cout << endl;
		}
		cout << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << KO[i].waga;
		}
		cout << " " << "<- wagi";
		cout << endl;

		for (i = 0; i < wierzcholki; i++)
		{
			koszta[i] = MAXINT;
			poprzednicy[i] = -1;
			odwiedzone[i] = false;
			kopiec[i] = pozycjaKopiec[i] = i;
		}


		czasStart();
		rozmiarKopca = wierzcholki;

		koszta[w] = 0;
		wezel = kopiec[0];
		kopiec[0] = kopiec[w];
		kopiec[w] = wezel;
		pozycjaKopiec[w] = 0;
		pozycjaKopiec[0] = w;

		for (i = 0; i < wierzcholki; i++)
		{
			korzen = kopiec[0];

			kopiec[0] = kopiec[--rozmiarKopca];
			pozycjaKopiec[kopiec[0]] = ojciec = 0;
			while (true)
			{
				lewySyn = ojciec + ojciec + 1;
				prawySyn = lewySyn + 1;
				if (lewySyn >= rozmiarKopca) break;
				kosztMin = koszta[kopiec[lewySyn]];
				synMin = lewySyn;
				if ((prawySyn < rozmiarKopca) && (kosztMin > koszta[kopiec[prawySyn]]))
				{
					kosztMin = koszta[kopiec[prawySyn]];
					synMin = prawySyn;
				}
				if (koszta[kopiec[ojciec]] <= kosztMin)
					break;
				wezel = kopiec[ojciec];
				kopiec[ojciec] = kopiec[synMin];
				kopiec[synMin] = wezel;
				pozycjaKopiec[kopiec[ojciec]] = ojciec;
				pozycjaKopiec[kopiec[synMin]] = synMin;
				ojciec = synMin;
			}

			odwiedzone[korzen] = true;

			for (l = 0; l < krawedzie; l++)
			{
				if (macierzIncydencji[korzen][l] != 0)
					for (j = 0; j < wierzcholki; j++)
						if (j != korzen && macierzIncydencji[j][l] == -1 && !odwiedzone[j] && (koszta[j] > koszta[korzen] + K[l].waga))
						{
							koszta[j] = koszta[korzen] + K[l].waga;
							poprzednicy[j] = korzen;

							for (syn = pozycjaKopiec[j]; syn; syn = ojciec)
							{
								ojciec = syn / 2;
								if (koszta[kopiec[ojciec]] <= koszta[kopiec[syn]])
									break;
								wezel = kopiec[ojciec];
								kopiec[ojciec] = kopiec[syn];
								kopiec[syn] = wezel;
								pozycjaKopiec[kopiec[ojciec]] = ojciec;
								pozycjaKopiec[kopiec[syn]] = syn;
							}
						}
			}
		}
		pobierzCzas();
		cout << endl;

		cout << "Najkrotsza droga z wierzcholka nr " << w << " do wierzcholka nr: " << t << endl;
		string displayText = "";
		string temp = "";
		int minCost = MAXINT;

		for (i = 0; i < wierzcholki; i++)
		{
			szerokosc = -2;

			if (koszta[t] == MAXINT || koszta[t]<0)
				cout << "Brak sciezki" << endl;
			else

			{
				for (j = i; j > -1; j = poprzednicy[j])
				{
					stos.push(j);
					szerokosc = szerokosc + 2;
				}

				int koncowyWierzcholek;
				while (!stos.empty())
				{
					koncowyWierzcholek = stos.top();
					temp += to_string(stos.top()) + " ";
					stos.pop();
				}
				//for (j = 0; j < wierzcholki - szerokosc; j++)
					temp += " waga sciezki";
				temp += "(" + to_string(koszta[i]);
				temp += ")";

				if (koncowyWierzcholek == t) {
					displayText = temp;
				}
				temp = "";
			}
		}

		cout << displayText;
		cout << endl << endl;
	}

	void FordaBellmanaLista(int w, int t)
	{
		int korzen, wezel, rozmiarKopca, ojciec, lewySyn, prawySyn, kosztMin, synMin, syn, *koszta, *poprzednicy, *kopiec, *pozycjaKopiec;
		Stos stos;
		int szerokosc, i, j;
		koszta = new int[wierzcholki];
		poprzednicy = new int[wierzcholki];
		odwiedzone = new bool[wierzcholki];
		kopiec = new int[wierzcholki];
		pozycjaKopiec = new int[wierzcholki];

		odwiedzone = new bool[wierzcholki];
		//for (i = 0; i < wierzcholki; i++)
		//{
		//	odwiedzone[i] = false;
		//}
		cout << endl << "Graf skierowany poczatkowy: " << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << "LS[" << i << "] =";
			e1 = listySasiedztwa[i];
			while (e1)
			{
				cout << setw(3) << e1->w << "(" << e1->waga << ") ";
				e1 = e1->nastepny;
			}
			cout << endl;
		}
		cout << endl;

		for (i = 0; i < wierzcholki; i++)
		{
			koszta[i] = MAXINT;
			poprzednicy[i] = -1;
			odwiedzone[i] = false;
			kopiec[i] = pozycjaKopiec[i] = i;
		}
		czasStart();
		for (int k = 1; k < 60; k++) {
			rozmiarKopca = wierzcholki;

			koszta[w] = 0;
			wezel = kopiec[0];
			kopiec[0] = kopiec[w];
			kopiec[w] = wezel;
			pozycjaKopiec[w] = 0;
			pozycjaKopiec[0] = w;

			for (i = 0; i < wierzcholki; i++)
			{
				korzen = kopiec[0];

				kopiec[0] = kopiec[--rozmiarKopca];
				pozycjaKopiec[kopiec[0]] = ojciec = 0;
				while (true)
				{
					lewySyn = ojciec + ojciec + 1;
					prawySyn = lewySyn + 1;
					if (lewySyn >= rozmiarKopca) break;
					kosztMin = koszta[kopiec[lewySyn]];
					synMin = lewySyn;
					if ((prawySyn < rozmiarKopca) && (kosztMin > koszta[kopiec[prawySyn]]))
					{
						kosztMin = koszta[kopiec[prawySyn]];
						synMin = prawySyn;
					}
					if (koszta[kopiec[ojciec]] <= kosztMin)
						break;
					wezel = kopiec[ojciec];
					kopiec[ojciec] = kopiec[synMin];
					kopiec[synMin] = wezel;
					pozycjaKopiec[kopiec[ojciec]] = ojciec;
					pozycjaKopiec[kopiec[synMin]] = synMin;
					ojciec = synMin;
				}

				odwiedzone[korzen] = true;

				if (listySasiedztwa[korzen] != NULL)
					for (e1 = listySasiedztwa[korzen]; e1; e1 = e1->nastepny)
						if (!odwiedzone[e1->w] && (koszta[e1->w] > koszta[korzen] + e1->waga))
						{
							koszta[e1->w] = koszta[korzen] + e1->waga;
							poprzednicy[e1->w] = korzen;

							for (syn = pozycjaKopiec[e1->w]; syn; syn = ojciec)
							{
								ojciec = syn / 2;
								if (koszta[kopiec[ojciec]] <= koszta[kopiec[syn]])
									break;
								wezel = kopiec[ojciec];
								kopiec[ojciec] = kopiec[syn];
								kopiec[syn] = wezel;
								pozycjaKopiec[kopiec[ojciec]] = ojciec;
								pozycjaKopiec[kopiec[syn]] = syn;
							}
						}
			}
		}
		pobierzCzas();
		cout << endl;

		cout << "Najkrotsza droga z wierzcholka nr " << w << " do wierzcholka nr: " << t << endl;
		string displayText = "";
		string temp = "";
		int minCost = MAXINT;

		for (i = 0; i < wierzcholki; i++)
		{
			szerokosc = -2;

			if (koszta[t] == MAXINT || koszta[t]<0)
				cout << "Brak sciezki" << endl;
			else

			{
				for (j = i; j > -1; j = poprzednicy[j])
				{
					stos.push(j);
					szerokosc = szerokosc + 2;
				}

				int koncowyWierzcholek;
				while (!stos.empty())
				{
					koncowyWierzcholek = stos.top();
					temp += to_string(stos.top()) + " ";
					stos.pop();
				}
				for (j = 0; j < wierzcholki - szerokosc; j++)
					temp += " waga sciezki";
				temp += "(" + to_string(koszta[i]);
				temp += ")";

				if (koncowyWierzcholek == t) {
					displayText = temp;
				}
				temp = "";
			}
		}

		cout << displayText;
		cout << endl << endl;
	}
	void FordaBellmanaMacierz(int w, int t)
	{
		int korzen, wezel, rozmiarKopca, ojciec, lewySyn, prawySyn, kosztMin, synMin, syn, *koszta, *poprzednicy, *kopiec, *pozycjaKopiec;
		Stos stos;
		int szerokosc, i, j, l;
		koszta = new int[wierzcholki];
		poprzednicy = new int[wierzcholki];
		odwiedzone = new bool[wierzcholki];
		kopiec = new int[wierzcholki];
		pozycjaKopiec = new int[wierzcholki];

		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}

		cout << "Graf skierowany poczatkowy: " << endl << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << i;

		}
		cout << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << setw(3) << i;
			for (int j = 0; j < krawedzieOdwrotne; j++)
				cout << setw(3) << macierzIncydencji[i][j];
			cout << endl;
		}
		cout << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << KO[i].waga;
		}
		cout << " " << "<- wagi";
		cout << endl;

		for (i = 0; i < wierzcholki; i++)
		{
			koszta[i] = MAXINT;
			poprzednicy[i] = -1;
			odwiedzone[i] = false;
			kopiec[i] = pozycjaKopiec[i] = i;
		}


		czasStart();
		rozmiarKopca = wierzcholki;

		koszta[w] = 0;
		wezel = kopiec[0];
		kopiec[0] = kopiec[w];
		kopiec[w] = wezel;
		pozycjaKopiec[w] = 0;
		pozycjaKopiec[0] = w;

		for (i = 0; i < wierzcholki; i++)
		{
			korzen = kopiec[0];

			kopiec[0] = kopiec[--rozmiarKopca];
			pozycjaKopiec[kopiec[0]] = ojciec = 0;
			while (true)
			{
				lewySyn = ojciec + ojciec + 1;
				prawySyn = lewySyn + 1;
				if (lewySyn >= rozmiarKopca) break;
				kosztMin = koszta[kopiec[lewySyn]];
				synMin = lewySyn;
				if ((prawySyn < rozmiarKopca) && (kosztMin > koszta[kopiec[prawySyn]]))
				{
					kosztMin = koszta[kopiec[prawySyn]];
					synMin = prawySyn;
				}
				if (koszta[kopiec[ojciec]] <= kosztMin)
					break;
				wezel = kopiec[ojciec];
				kopiec[ojciec] = kopiec[synMin];
				kopiec[synMin] = wezel;
				pozycjaKopiec[kopiec[ojciec]] = ojciec;
				pozycjaKopiec[kopiec[synMin]] = synMin;
				ojciec = synMin;
			}

			odwiedzone[korzen] = true;

			for (l = 0; l < krawedzie; l++)
			{
				if (macierzIncydencji[korzen][l] != 0)
					for (j = 0; j < wierzcholki; j++)
						if (j != korzen && macierzIncydencji[j][l] == -1 && !odwiedzone[j] && (koszta[j] > koszta[korzen] + K[l].waga))
						{
							koszta[j] = koszta[korzen] + K[l].waga;
							poprzednicy[j] = korzen;

							for (syn = pozycjaKopiec[j]; syn; syn = ojciec)
							{
								ojciec = syn / 2;
								if (koszta[kopiec[ojciec]] <= koszta[kopiec[syn]])
									break;
								wezel = kopiec[ojciec];
								kopiec[ojciec] = kopiec[syn];
								kopiec[syn] = wezel;
								pozycjaKopiec[kopiec[ojciec]] = ojciec;
								pozycjaKopiec[kopiec[syn]] = syn;
							}
						}
			}
		}
		pobierzCzas();
		cout << endl;

		cout << "Najkrotsza droga z wierzcholka nr " << w << " do wierzcholka nr: " << t << endl;
		string displayText = "";
		string temp = "";
		int minCost = MAXINT;

		for (i = 0; i < wierzcholki; i++)
		{
			szerokosc = -2;

			if (koszta[t] == MAXINT || koszta[t]<0)
				cout << "Brak sciezki" << endl;
			else

			{
				for (j = i; j > -1; j = poprzednicy[j])
				{
					stos.push(j);
					szerokosc = szerokosc + 2;
				}

				int koncowyWierzcholek;
				while (!stos.empty())
				{
					koncowyWierzcholek = stos.top();
					temp += to_string(stos.top()) + " ";
					stos.pop();
				}
				for (j = 0; j < wierzcholki - szerokosc; j++)
					temp += " waga sciezki";
				temp += "(" + to_string(koszta[i]);
				temp += ")";

				if (koncowyWierzcholek == t) {
					displayText = temp;
				}
				temp = "";
			}
		}

		cout << displayText;
		cout << endl << endl;
	}

	void kruskalLista()/////////////
	{
		int w, i;
		Krawedz k;
		priority_queue <Krawedz, vector<Krawedz>, Krawedz> kolejka;
		DrzewoSpinajace *drzewo = new DrzewoSpinajace(wierzcholki, krawedzie);
		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}
		cout << endl << "Graf nieskierowany poczatkowy: " << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << "LS[" << i << "] =";
			e1 = grafNieskierowanyL[i];
			while (e1)
			{
				cout << setw(3) << e1->w << "(" << e1->waga << ") ";
				e1 = e1->nastepny;
			}
			cout << endl;
		}
		cout << endl;
		czasStart();
		w = 0;
		odwiedzone[w] = true;

		for (i = 1; i < wierzcholki; i++)
		{

			for (e1 = grafNieskierowanyL[w]; e1; e1 = e1->nastepny)
			{
				if (!odwiedzone[e1->w])
				{
					k.wp = w;
					k.wk = e1->w;
					k.waga = e1->waga;
					kolejka.push(k);
				}
			}
			do
			{
				k = kolejka.top();
				kolejka.pop();

			} while (odwiedzone[k.wk]);

			drzewo->dodajKrawedz(k);
			odwiedzone[k.wk] = true;
			w = k.wk;

		}

		pobierzCzas();
		cout << endl;
		cout << "Minimalne drzewo rozpinajace:";
		drzewo->wyswietl();
		delete drzewo;
	}
	void kruskalMacierz()/////////
	{

		int w, i, j, g;
		Krawedz krawedz;
		priority_queue <Krawedz, vector<Krawedz>, Krawedz> kolejka;
		DrzewoSpinajace *drzewo = new DrzewoSpinajace(wierzcholki, krawedzie);
		odwiedzone = new bool[wierzcholki];
		for (i = 0; i < wierzcholki; i++)
		{
			odwiedzone[i] = false;
		}

		cout << "Graf nieskierowany na poczatkowy: " << endl << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << i;

		}
		cout << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << setw(3) << i;
			for (int j = 0; j < krawedzieOdwrotne; j++)
				cout << setw(3) << grafNieskierowanyM[i][j];
			cout << endl;
		}
		cout << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << KO[i].waga;
		}
		cout << " " << "<- wagi";
		cout << endl;
		czasStart();

		w = 0;
		odwiedzone[w] = true;
		for (i = 1; i < wierzcholki; i++)
		{
			for (g = 0; g < krawedzieOdwrotne; g++)
			{

				if (grafNieskierowanyM[w][g] != 0)
					for (j = 0; j < wierzcholki; j++)
						if (j != w && grafNieskierowanyM[j][g] != 0 && !odwiedzone[j])
						{
							krawedz.wp = w;
							krawedz.wk = j;
							krawedz.waga = KO[g].waga;
							kolejka.push(krawedz);

						}
			}
			do
			{
				krawedz = kolejka.top();
				kolejka.pop();

			} while (odwiedzone[krawedz.wk]);

			drzewo->dodajKrawedz(krawedz);
			odwiedzone[krawedz.wk] = true;
			w = krawedz.wk;

		}
		
		pobierzCzas();
		cout << endl << endl;
		cout << "Minimalne drzewo rozpinajace:";
		drzewo->wyswietl();
		delete drzewo;
	}

	void primLista()
	{
		int w, i;
		Krawedz k;
		vector <Krawedz> kolejka;
		DrzewoSpinajace *drzewo = new DrzewoSpinajace(wierzcholki, krawedzie);
		set<int> odwiedzone = set<int>();
		
		cout << endl << "Graf nieskierowany poczatkowy: " << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << "LS[" << i << "] =";
			e1 = grafNieskierowanyL[i];
			while (e1)
			{
				cout << setw(3) << e1->w << "(" << e1->waga << ") ";
				e1 = e1->nastepny;
			}
			cout << endl;
		}
		cout << endl;
		czasStart();
		w = 0;
		
		for (i = 1; i < wierzcholki; i++)
		{

			for (e1 = grafNieskierowanyL[w]; e1; e1 = e1->nastepny)
			{
					k.wp = w;
					k.wk = e1->w;
					k.waga = e1->waga;
					kolejka.push_back(k);
			}
			
			//drzewo->dodajKrawedz(k);
			if (w == 0)
				odwiedzone.insert(minimalnyWierzcholek(kolejka).wp);
			
			Krawedz krawedz = znajdzMinimum(odwiedzone, kolejka);
			odwiedzone.insert(krawedz.wp);
			odwiedzone.insert(krawedz.wk);
			drzewo->dodajKrawedz(krawedz);
			w = krawedz.wk;
			
			//w = k.wk;

		}

		pobierzCzas();
		cout << endl;
		cout << "Minimalne drzewo rozpinajace:";
		drzewo->wyswietl();
		delete drzewo;
	}

	void primMacierz()
	{
		int w, i, j, g;
		Krawedz krawedz;
		vector <Krawedz> kolejka;
		DrzewoSpinajace *drzewo = new DrzewoSpinajace(wierzcholki, krawedzie);
		set<int> odwiedzone = set<int>();

		cout << "Graf nieskierowany poczatkowy: " << endl << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << i;

		}
		cout << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << setw(3) << i;
			for (int j = 0; j < krawedzieOdwrotne; j++)
				cout << setw(3) << grafNieskierowanyM[i][j];
			cout << endl;
		}
		cout << endl;
		cout << "   ";
		for (i = 0; i < krawedzieOdwrotne; i++)
		{
			cout << setw(3) << KO[i].waga;
		}
		cout << " " << "<- wagi";
		cout << endl;
		czasStart();

		w = 0;
		for (i = 1; i < wierzcholki; i++)
		{
			for (g = 0; g < krawedzieOdwrotne; g++)
			{

				if (grafNieskierowanyM[w][g] != 0)
					for (j = 0; j < wierzcholki; j++)
						if (j != w && grafNieskierowanyM[j][g] != 0)// && !odwiedzone[j])
						{
							krawedz.wp = w;
							krawedz.wk = j;
							krawedz.waga = KO[g].waga;
							kolejka.push_back(krawedz);

						}
			}

			if(w == 0)
				odwiedzone.insert(minimalnyWierzcholek(kolejka).wp);

			/*for (i = 0; i < wierzcholki; i++)
			{*/
				Krawedz krawedz = znajdzMinimum(odwiedzone, kolejka);
				odwiedzone.insert(krawedz.wp);
				odwiedzone.insert(krawedz.wk);
				drzewo->dodajKrawedz(krawedz);
				w = krawedz.wk;
			//}
		}
		
		pobierzCzas();
		cout << endl << endl;
		cout << "Minimalne drzewo rozpinajace:";
		drzewo->wyswietl();
		delete drzewo;
	}

	Krawedz minimalnyWierzcholek(vector<Krawedz> &krawedzie) {
		int minimum = INT_MAX;
		Krawedz krawedzMinimalna;

		for (Krawedz &krawedz : krawedzie) {
			if (krawedz.waga < minimum) {
				minimum = krawedz.waga;
				krawedzMinimalna = krawedz;
			}
		}
		return krawedzMinimalna;
	}

	Krawedz& znajdzMinimum(set<int> &odwiedzone, vector<Krawedz> &krawedzie) {
		int minimum = INT_MAX;
		Krawedz krawedzMinimalna;

		for (Krawedz &krawedz : krawedzie) {
			if (odwiedzone.find(krawedz.wp) != odwiedzone.end() && !(odwiedzone.find(krawedz.wk) != odwiedzone.end())
				|| !(odwiedzone.find(krawedz.wp) != odwiedzone.end()) && odwiedzone.find(krawedz.wk) != odwiedzone.end()) {

				if (krawedz.waga < minimum) {
					minimum = krawedz.waga;
					krawedzMinimalna = krawedz;
				}
			}
		}
		return krawedzMinimalna;
	}

	void wyswietl()
	{
		int i;
		cout << "   ";
		for (i = 0; i < krawedzie; i++)
		{
			cout << setw(3) << i;

		}
		cout << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << setw(3) << i;
			for (int j = 0; j < krawedzie; j++)
				cout << setw(3) << macierzIncydencji[i][j];
			cout << endl;
		}
		cout << endl;
		cout << "   ";
		for (i = 0; i < krawedzie; i++)
		{
			cout << setw(3) << K[i].waga;
		}

		cout << " " << "<- wagi";
		cout << endl << endl << endl;
		for (i = 0; i < wierzcholki; i++)
		{
			cout << "LS[" << i << "] =";
			e1 = listySasiedztwa[i];
			while (e1)
			{
				cout << setw(3) << e1->w << "(" << e1->waga << ") ";
				e1 = e1->nastepny;
			}
			cout << endl;
		}
	}
};
int main()
{
	srand(time(NULL));
	int wybor, w, g, b, o;
	Graf *graf;
	bool naPoczatek = true;
	while (naPoczatek)
	{
		cout << "Stworz graf" << endl << "1. Wczytaj z pliku" << endl << "2. Wygeneruj graf losowo" << endl;
		cin >> wybor;
		system("cls");
		switch (wybor)
		{
		case 1:
		{
			naPoczatek = false;
			ElementListy *e1;
			string s, nazwa;
			int a = 0;
			int krawedzie, wierzcholki;
			cout << "Podaj nazwe pliku" << endl;
			cin >> nazwa;
			nazwa = nazwa + ".txt";
			ifstream plik(nazwa);
			if (!plik)
			{
				cout << "Nie mozna otworzyc pliku" << endl;
				naPoczatek = true;

			}

			else
			{

				{
					plik >> krawedzie >> wierzcholki;
					if (!plik || krawedzie < wierzcholki - 1 || wierzcholki <= 1 || krawedzie >(wierzcholki*(wierzcholki - 1)))
					{
						cout << "Liczba wierzcholkow lub krawedzi jest nieprawidlowa" << endl;

						naPoczatek = true;
					}

					else
					{
						graf = new Graf(wierzcholki, krawedzie);
						while (!plik.eof())
						{

							plik >> graf->K[a].wp >> graf->K[a].wk >> graf->K[a].waga;
							if (graf->K[a].wp >= wierzcholki || graf->K[a].wk >= wierzcholki || graf->K[a].waga < 1)
							{
								cout << "Zle podane krawedzie" << endl;
								naPoczatek = true;
								break;
							}
							if (plik)
								a++;

							else
							{
								cout << "Zle podane krawedzie" << endl;
								naPoczatek = true;
								break;
							}
						}
						if (a == krawedzie)
						{
							for (int i = 0; i < wierzcholki; i++)
								graf->macierzIncydencji[i] = new int[krawedzie];
							for (int i = 0; i < wierzcholki; i++)
								for (int j = 0; j < krawedzie; j++)
								{
									graf->macierzIncydencji[i][j] = 0;
								}

							for (int i = 0; i < wierzcholki; i++)
								graf->listySasiedztwa[i] = NULL;

							for (int i = 0; i < krawedzie; i++)
							{
								int wp = graf->K[i].wp;
								int wk = graf->K[i].wk;
								e1 = new ElementListy;
								e1->w = wk;
								e1->waga = graf->K[i].waga;
								e1->nastepny = graf->listySasiedztwa[wp];
								graf->listySasiedztwa[wp] = e1;
								graf->macierzIncydencji[wp][i] = 1;
								graf->macierzIncydencji[wk][i] = -1;
							}
							plik.close();
							graf->grafNieskierowany();
							if (!(graf->czySpojny()))
							{
								cout << "Graf, zbudowany z pliku jest niespojny" << endl;
								naPoczatek = true;
								delete graf;
							}
							else
								system("cls");
							while (!naPoczatek)
							{
								cout << "Graf " << endl
									 << "Wierzcholki: " << graf->getWierzcholki() << endl
									 << "Krawedzie: " << graf->getKrawedzie() << endl
									 << "1. Wyswietl" << endl
									 << "2. Forda-Bellmana - macierz incydencji " << endl
									 << "3. Forda-Bellmana - listy sasiedztwa" << endl
									 << "4. Dijkstra - macierz incydencji" << endl 
									 << "5. Dijkstra - listy sasiedztwa" << endl 
									 << "6. Kruskal - macierz incydencji " << endl 
									 << "7. Kruskal - listy sasiedztwa" << endl 
									 << "8. Wroc do tworzenia grafu" << endl 
									 << "9. Prim - macierz incydencji" << endl 
									 << "10. Prim - listy sasiedztwa" << endl;
								cin >> wybor;
								switch (wybor)
								{
								case 1: {
									graf->wyswietl();

								}break;
								case 2:
								{
									while (true)
									{
										cout << "Podaj numer wierzcholka startowego" << endl;
										cin >> b;
										cout << "Podaj numer wierzcholka koncowego" << endl;
										cin >> o;

										if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
											break;
										}

										if (b >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
										}

										if (o >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
										}
									}
									cout << endl;
									graf->FordaBellmanaMacierz(b, o);

								}break;
								case 3:
								{
									while (true)
									{
										cout << "Podaj numer wierzcholka startowego" << endl;
										cin >> b;
										cout << "Podaj numer wierzcholka koncowego" << endl;
										cin >> o;

										if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
											break;
										}

										if (b >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
										}

										if (o >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
										}
									}
									cout << endl;
									graf->FordaBellmanaLista(b, o);

								}break;
								case 4: {
									while (true)
									{
										cout << "Podaj numer wierzcholka startowego" << endl;
										cin >> b;
										cout << "Podaj numer wierzcholka koncowego" << endl;
										cin >> o;

										if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
											break;
										}

										if (b >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
										}

										if (o >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
										}
									}
									cout << endl;
									graf->DijkstraMacierz(b, o);

								}break;
								case 5: {
									while (true)
									{
										cout << "Podaj numer wierzcholka startowego" << endl;
										cin >> b;
										cout << "Podaj numer wierzcholka koncowego" << endl;
										cin >> o;

										if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
											break;
										}

										if (b >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
										}

										if (o >= graf->getWierzcholki()) {
											cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
										}
									}
									cout << endl;
									graf->DijkstraLista(b, o);

								}break;
								case 6: {
									graf->kruskalMacierz();

								}break;
								case 7: {
									graf->kruskalLista();

								}break;
								case 8: {
									delete graf;
									system("cls");
									naPoczatek = true;

								}break;
								case 9: {
									graf->primMacierz();

								}break;
								case 10: {
									graf->primLista();

								}break;
								}
							}
						}
						else if (naPoczatek == false)
						{
							naPoczatek = true;
							cout << "Za malo danych w stosunku do podanych krawedzi" << endl;
						}
					}
				}
			}
		}break;
		case 2: {
			naPoczatek = false;
			while (true)
			{
				cout << "Podaj liczbe wierzcholkow" << endl;
				cin >> w;
				if (w > 1) break;
				else
					cout << "Liczba wierzcholkow musi byc wieksza od 1" << endl;
			}
			
			int maxK = w*(w - 1);
			double minG = ceil((((double)w - 1) * 100) / (double)maxK);

			while (true)
			{

				cout << "Podaj gestosc (minimum " << minG << " %" << ")" << endl;
				cin >> g;
				if (g < minG || g > 100) cout << "Podaj prawidlowa wartosc"
					<< endl << "Aby graf byl spojny, gestosc musi wynosic przynajmniej " << minG << " %" << endl;
				else
					break;
			}
			double krawedzie = ceil((maxK*g) / 100);
			graf = new Graf(w, krawedzie);
			graf->losowyGraf();
			system("cls");

			while (!naPoczatek)
			{

				cout << "Graf " << endl 
					<< "Wierzcholki: " << graf->getWierzcholki() << endl 
					<< "Krawedzie: " << graf->getKrawedzie() << endl 
					<< "Gestosc: " << g << endl
					<< "1. Wyswietl" << endl 
					<< "2. Forda-Bellmana - macierz incydencji " << endl 
					<< "3. Forda-Bellmana - listy sasiedztwa" << endl 
					<< "4. Dijkstra - macierz incydencji" << endl 
					<< "5. Dijkstra - listy sasiedztwa" << endl 
					<< "6. Kruskal - macierz incydencji" << endl 
					<< "7. Kruskal - listy sasiedztwa" << endl 
					<< "8. Wroc do tworzenia grafu" << endl 
					<< "9. Prim - macierz incydencji" << endl 
					<< "10. Prim - listy sasiedztwa" << endl;
				cin >> wybor;
				switch (wybor)
				{
				case 1: {
					graf->wyswietl();
					cout << endl;

				}break;
				case 2:
				{
					while (true)
					{
						cout << "Podaj numer wierzcholka startowego" << endl;
						cin >> b;
						cout << "Podaj numer wierzcholka koncowego" << endl;
						cin >> o;

						if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
							break;
						}

						if (b >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
						}

						if (o >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
						}
					}
					cout << endl;
					graf->FordaBellmanaMacierz(b, o);
				}break;
				case 3:
				{
					while (true)
					{
						cout << "Podaj numer wierzcholka startowego" << endl;

						cin >> b;
						cout << "Podaj numer wierzcholka koncowego" << endl;
						cin >> o;

						if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
							break;
						}

						if (b >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
						}

						if (o >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
						}
					}
					cout << endl;
					graf->FordaBellmanaLista(b, o);
				}break;
				case 4: {
					while (true)
					{
						cout << "Podaj numer wierzcholka startowego" << endl;
						cin >> b;
						cout << "Podaj numer wierzcholka koncowego" << endl;
						cin >> o;

						if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
							break;
						}

						if (b >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
						}

						if (o >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
						}
					}
					cout << endl;
					graf->DijkstraMacierz(b, o);

				}break;
				case 5: {
					while (true)
					{
						cout << "Podaj numer wierzcholka startowego" << endl;

						cin >> b;
						cout << "Podaj numer wierzcholka koncowego" << endl;
						cin >> o;

						if (b < graf->getWierzcholki() && o < graf->getWierzcholki()) {
							break;
						}

						if (b >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka startowego" << endl;
						}

						if (o >= graf->getWierzcholki()) {
							cout << "Podaj prawidlowy numer wierzcholka koncowego" << endl;
						}
					}
					cout << endl;
					graf->DijkstraLista(b, o);

				}break;
				case 6: {
					graf->kruskalMacierz();

				}break;
				case 7: {
					graf->kruskalLista();

				}break;
				case 8: {
					delete graf;
					system("cls");
					naPoczatek = true;

				}break;
				case 9: {
					graf->primMacierz();

				}break;
				case 10: {
					graf->primLista();

				}break;
				}
			}

		}break;
		}
	}
	system("pause");
	return 0;
}