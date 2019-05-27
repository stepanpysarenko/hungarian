// hungarian.cpp : main project file.

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <limits>

using namespace System;

struct elem {
	int i;
	int j;
};


class Hungarian {

public:

	void Input() {

		int i, j;
		String^ buffer;
		bool err;
		cellsize = 0;
		
		do {
			Console::Write("Количество строк: ");
			try {
				buffer = Console::ReadLine();
				m = Convert::ToUInt32(buffer);
				if (m <= 1)  Console::WriteLine("Значение должно быть больше 1");
			}
			catch(...) { Console::WriteLine("Ошибка ввода"); }
		} while (m <= 1);
		
		do {
			Console::Write("Количество столбцов: ");
			try {
				buffer = Console::ReadLine();
				n = Convert::ToUInt32(buffer);
				if (n <= 1)  Console::WriteLine("Значение должно быть больше 1");
			}
			catch(...) { Console::WriteLine("Ошибка ввода"); }
		} while (n <= 1);

		// Инициализация матрицы и векоров заданных размеров
		A = new int [m];
		B = new int [n];
		C = new int* [m];
		for (i = 0; i < m; i++)
			C[i] = new int [n];

		Console::WriteLine("Размерность задачи {0} x {1}", m, n);
		Console::WriteLine("\nЭлементы матрицы стоимостей:");
		
		for (i = 0; i < m; i++) {
			for (j = 0; j < n; j++) {			
				do {
					Console::Write("[" + (i+1) + "][" + (j+1) + "] = ");
					try {
						buffer = Console::ReadLine();
						C[i][j] = Convert::ToInt32(buffer);
						err = false;
					}
					catch(...) {
						Console::WriteLine("Ошибка ввода");
						err = true;
					}
					if (Convert::ToUInt32(buffer->Length) > cellsize + 1)
						cellsize = buffer->Length + 1;
				} while (err);
			}
			Console::WriteLine();
		}

		Console::WriteLine("\nВектор запасов (A):");
		for (i = 0; i < m; i++) {	
			do {
				Console::Write("[" + (i+1) + "] = ");
				try {
					buffer = Console::ReadLine();
					A[i] = Convert::ToInt32(buffer);
					err = false;
				}
				catch(...) {
					Console::WriteLine("Ошибка ввода");
					err = true;
				}
				if (Convert::ToUInt32(buffer->Length) > cellsize + 1)
						cellsize = buffer->Length + 1;
			} while (err);
		}

		Console::WriteLine("\nВектор потребностей (B):");
		for (i = 0; i < n; i++) {	
			do {
				Console::Write("[" + (i+1) + "] = ");
				try {
					buffer = Console::ReadLine();
					B[i] = Convert::ToInt32(buffer);
					err = false;
				}
				catch(...) {
					Console::WriteLine("Ошибка ввода");
					err = true;
				}
				if (Convert::ToUInt32(buffer->Length) > cellsize + 1)
						cellsize = buffer->Length + 1;
			} while (err);
		}
		
		Console::WriteLine("\nВходные данные:\n");
		for (i = 0; i < m; i++) {
			Console::Write(" ");
			for (j = 0; j < n; j++)
				Console::Write(" {0,-" + (cellsize) + ":D}", C[i][j]);
			Console::WriteLine("| {0,-" + (cellsize+1) + ":D}", A[i]);
		}
		Console::Write(" ");
		for (j = 0; j < (n*(cellsize+1)); j++)
			Console::Write("-");
		Console::Write("\n  ");
		for (j = 0; j < n; j++)
			Console::Write("{0,-" + (cellsize+1) + ":D}", B[j]);
		Console::Write("\n\n");

	}

	void Solve() {

		int i, j, min;
		bool zero_found;

		//if (!IsBalanced()) {
		//	Console::WriteLine("Условие баланса не выполняется!");
		//	return;
		//}
		MakeClosed();

		// Инициализация дополнительных матриц и векторов
		ZERO = new int* [n];
		CSTART = new int* [n];
		X = new int* [n];
		ROW0 = new int [n];
		COL0 = new int [n];
		DELTAA = new int [m];
		DELTAB = new int [n];
		for (i = 0; i < m; i++) {
			ZERO[i] = new int [n];
			CSTART[i] = new int [n];
			X[i] = new int [n];
			for (j = 0; j < n; j++) {
				ZERO[i][j] = X[i][j] = COL0[j] = 0;
			}
			ROW0[i] = 0;
		}

		// Копирование изначальных данных
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				CSTART[i][j] = C[i][j];
		for (j = 0; j < n; j++)
			DELTAB[j] = B[j];
		for (i = 0; i < m; i++)
			DELTAA[i] = A[i];

		Console::WriteLine("========== Предварительный этап ==========\n");

		// Приведение матрицы по столбцам
		for (j = 0; j < n; j++) {
			min = C[0][j];
			for (i = 1; i < m; i++) 
				min = C[i][j] < min ? C[i][j] : min;
			for (i = 0; i < m; i++)
				C[i][j] -= min;
		}

		// Приведение матрицы по строкам
		for (i = 0; i < m; i++) {
			min = C[i][0];
			for (j = 1; j < n; j++) 
				min = C[i][j] < min ? C[i][j] : min;
			for (j = 0; j < n; j++)
				C[i][j] -= min;
		}
	
		Console::WriteLine("Приведенная матрица C0:");
		ShowC();

		// Построение начальной матрицы перевозок
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				if (!C[i][j]) {	
					if (DELTAA[i] >= DELTAB[j]) X[i][j] = DELTAB[j];
					if (DELTAA[i] < DELTAB[j]) X[i][j] = DELTAA[i];						
					DELTAB[j] -= X[i][j]; //----------------------
					DELTAA[i] -= X[i][j]; //-----------------------
				}
				else
					X[i][j] = 0;

		Console::WriteLine("Матрица X0:");
		ShowX();
		Console::WriteLine("delta = {0}\n", delta());

		if (!delta()) {
			ShowResult();
			return;
		}

		iteration = 0;
		// Итерация ==============================================================================================
		while (delta()) {

		Console::WriteLine("========== Итерация {0} ==========", ++iteration);

		// Отмечаем столбцы "+"
		for (j = 0; j < n; j++)
			if (!DELTAB[j]) COL0[j] = 1;
			else COL0[j] = 0;

		// Обнуление разметки строк и нулей
		for (i = 0; i < m; i++) {
			ROW0[i] = 0;
			for (j = 0; j < n; j++)
				ZERO[i][j] = 0;
		}
	
		//'=1  *=-1

		// 1-3 этап
		adjust = false;
		while(!adjust) {

			// 1 этап

			Console::WriteLine("===== 1 этап =====");		
			//ShowC();

			do {
				zero_found = false;
				for (j = 0; j < n; j++) {
					if (COL0[j]) continue;
					for (i = 0; i < m; i++) {
						if (!C[i][j] && !COL0[j] && !ROW0[i]) {
							zero_found = true;
							break;	
						}
					}
					if (zero_found) break;
				}
				if (zero_found) {
					adjust = false;
					adjust = ArraySearch(true, -1, j);
				}
				if (adjust) zero_found = false;
			} while(zero_found);
			
			// Конец 1 этапа

			Console::WriteLine("\nМатрица C{0}:", iteration);
			ShowC();
			//Console::ReadKey(true);//////////

			// 3 этап

			if (!adjust) {	
				Console::WriteLine("===== 3 этап =====");
				int h = std::numeric_limits<int>::max();
				Console::Write("\nh = min [ ");
				for (i = 0; i < m; i++)
					for (j = 0; j < n; j++)
						if (!ROW0[i] && !COL0[j]) { /////////
							Console::Write("{0}, ", C[i][j]);
							h = C[i][j] < h ? C[i][j] : h;
						}
				Console::WriteLine(" ] = {0}\n", h);

				for (i = 0; i < m; i++)
					for (j = 0; j < n; j++)
						if (COL0[j]) C[i][j] += h;
						

				for (j = 0; j < n; j++)
					for (i = 0; i < m; i++)
						if (!ROW0[i]) C[i][j] -= h;
						
				// Обнуление разметки строк и нулей
				for (i = 0; i < m; i++) {
					ROW0[i] = 0;
					for (j = 0; j < n; j++) {
						ZERO[i][j] = 0;
						if (!DELTAB[j]) COL0[j] = 1;
						else COL0[j] = 0;
					}
				}

				Console::WriteLine("Матрица C{0}:", iteration);
				ShowC();
				//Console::ReadKey(true);////////////////////
			}

			// Конец 3 этапа
		
		}	
		// Конец пары 1-3 этап
		
		// 2 етап
		Console::WriteLine("===== 2 этап =====");

		chain.clear();
		for (j = 0; j < n; j++) {
			for (i = 0; i < m; i++) {
				if (ZERO[i][j] > 0 && DELTAA[i] > 0) {
					AddToChain(i, j);
					break;
				}
			}
			if (chain.size()) break;
		}
		BuildChain(true, i, j);
				

		Console::Write("\nЦепочка: ");
		for (i = 0; i < chain.size(); i++)
			if (i != chain.size()-1) Console::Write("O({0},{1}) - ", (chain[i].i + 1), (chain[i].j + 1));
			else Console::Write("O({0},{1})", (chain[i].i + 1), (chain[i].j + 1));
		Console::WriteLine("\n");

		Console::Write("teta = min [ ");
		min = std::numeric_limits<int>::max();
		for (i = 1; i < chain.size(); i+=2) {
			min = X[chain[i].i][chain[i].j] < min ? X[chain[i].i][chain[i].j] : min;
			Console::Write("{0}, ", X[chain[i].i][chain[i].j]);
		}
		min = DELTAA[chain[0].i] < min ? DELTAA[chain[0].i] : min;
		min = DELTAB[chain[chain.size()-1].j] < min ? DELTAB[chain[chain.size()-1].j] : min;
		Console::Write("{0}, ", DELTAA[chain[0].i]);
		Console::Write("{0}", DELTAB[chain[chain.size()-1].j]);
		Console::WriteLine(" ] = {0}\n", min);

		for (i = 0; i < chain.size(); i++) {
			if ((i+1)%2) X[chain[i].i][chain[i].j] += min;
			else X[chain[i].i][chain[i].j] -= min;
		}

		// Невязка по столбцам
		for (j = 0; j < n; j++) {
			DELTAB[j] = B[j];
			for (i = 0; i < m; i++)
				DELTAB[j] -= X[i][j];
		}

		// Невязка по строкам
		for (i = 0; i < m; i++) {
			DELTAA[i] = A[i];
			for (j = 0; j < n; j++)
				DELTAA[i] -= X[i][j];
		}
		// Конец 2 этапа

		Console::WriteLine("\nМатрица X{0}:", iteration);
		ShowX();
		Console::WriteLine("delta = {0}\n", delta());

		//Console::ReadKey(true);//////////

		} 
		// Конец итерации ==============================================================================================

		ShowResult();
		return;
	}


private:


	bool ArraySearch(bool column, int row, int col) {
		if (column)	{
			for (int i = 0; i < m; i++) {
				if (i == row) continue;
				if (!C[i][col] && !ZERO[i][col]) {
					ZERO[i][col] = 1;
					if (!DELTAA[i]) {
						ROW0[i] = 1;
						return ArraySearch(!column, i, col);
					}
					else return true;
				}
			}
		}
		else {
			for (int j = 0; j < n; j++)	{
				if (!COL0[j]) continue;
				if (!C[row][j] && X[row][j]) {
					COL0[j] = 0;
					ZERO[row][j] = -1;					
					return ArraySearch(!column, row, j);
				}
			}	
		}
		return false;
	}

	void BuildChain(bool column, int row, int col) {
		elem element;
		if (column)	{
			for (int i = 0; i < m; i++) {
				if (i == row) continue;
				if (ZERO[i][col] == -1) {
					AddToChain(i, col);
					BuildChain(!column, i, col);
					break;
				}
			}
		}
		else {
			for (int j = 0; j < n; j++)	{
				if (j == col) continue;
				if (ZERO[row][j] == 1) {
					AddToChain(row, j);
					BuildChain(!column, row, j);
					break;
				}
			}
		}
		return;
	}

	void AddToChain(int i, int j) {
		elem element;
		element.i = i;
		element.j = j;
		chain.push_back(element);
		return;
	}

	bool IsBalanced() {
		int sum = 0;
		for (int i = 0; i < m; i++)
			sum += A[i];
		for (int j = 0; j < n; j++)
			sum -= B[j];
		if(!sum) return true;
		return false;
	}

	void MakeClosed() {
		int i, j, sum_a = 0, sum_b = 0, delta;
		for (i = 0; i < m; i++)
			sum_a += A[i];
		for (j = 0; j < n; j++)
			sum_b += B[j];

		if (sum_a == sum_b) return;

		A0 = new int [m];
		B0 = new int [n];
		C0 = new int* [m];
		for (i = 0; i < m; i++) {
			C0[i] = new int [n];
			A0[i] = A[i];
			for (j = 0; j < n; j++) {
				C0[i][j] = C[i][j];
				B0[j] = B[j];
			}
		}

		if (sum_a > sum_b) n++;
		else m++;

		delete[] A;
		delete[] B;
		delete[] C;

		A = new int [m];
		B = new int [n];
		C = new int* [m];
		for (i = 0; i < m; i++) {
			C[i] = new int [n];
		}

		if(sum_a > sum_b) {
			for (i = 0; i < m; i++) {
				A[i] = A0[i];
				for (j = 0; j < n-1; j++) {
					C[i][j] = C0[i][j];
					B[j] = B0[j];
				}
			}
			for (i = 0; i < m; i++)
				C[i][n-1] = 0;
			B[n-1] = sum_a - sum_b;
		}
		else {
			for (i = 0; i < m-1; i++) {
				A[i] = A0[i];
				for (j = 0; j < n; j++) {
					C[i][j] = C0[i][j];
					B[j] = B0[j];
				}
			}
			for (j = 0; j < n; j++)
				C[m-1][j] = 0;
			A[m-1] = sum_b - sum_a;
		}

		Console::WriteLine("\nПреобразование в закрытую модель транспортной задачи:\n");
		for (i = 0; i < m; i++) {
			Console::Write(" ");
			for (j = 0; j < n; j++)
				Console::Write(" {0,-" + (cellsize) + ":D}", C[i][j]);
			Console::WriteLine("| {0,-" + (cellsize+1) + ":D}", A[i]);
		}
		Console::Write(" ");
		for (j = 0; j < (n*(cellsize+1)); j++)
			Console::Write("-");
		Console::Write("\n  ");
		for (j = 0; j < n; j++)
			Console::Write("{0,-" + (cellsize+1) + ":D}", B[j]);
		Console::Write("\n\n");

		return;
	}

	int delta() {
		int i, j, delta = 0;
		for (i = 0; i < m; i++)
			delta += A[i];
		for (j = 0; j < n; j++)
			delta += B[j];
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				delta -= 2 * X[i][j];
		return delta;
	}

	void ShowC() {
		int i, j;
		Console::WriteLine();

		Console::Write(" ");
		for (j = 0; j < n; j++)
			if (COL0[j]) Console::Write("{0,-" + (cellsize+1) + ":S}", " +");
			else Console::Write("{0,-" + (cellsize+1) + ":S}", "  ");
		Console::Write("\n ");
		for (j = 0; j < (n*(cellsize+1)); j++)
			Console::Write("-");
		Console::WriteLine();

		for (i = 0; i < m; i++) {
			Console::Write(" ");
			for (j = 0; j < n; j++) {
				if (ZERO[i][j] > 0)
					Console::Write("`{0,-" + (cellsize) + ":D}", C[i][j]);
				else if (ZERO[i][j] < 0)
					Console::Write("*{0,-" + (cellsize) + ":D}", C[i][j]);
				else
					Console::Write(" {0,-" + (cellsize) + ":D}", C[i][j]);
			}
			if (ROW0[i]) Console::WriteLine("| +");
			else Console::WriteLine("|");
		}

		Console::WriteLine();
		return;
	}

	void ShowX() {
		int i, j;
		Console::WriteLine();
		for (i = 0; i < m; i++) {
			Console::Write(" ");
			for (j = 0; j < n; j++)
				Console::Write(" {0,-" + (cellsize) + ":D}", X[i][j]);
			Console::WriteLine("| {0,-" + (cellsize+1) + ":D}", DELTAA[i]);
		}
		Console::Write(" ");
		for (j = 0; j < (n*(cellsize+1)); j++)
			Console::Write("-");
		Console::Write("\n  ");
		for (j = 0; j < n; j++)
			Console::Write("{0,-" + (cellsize+1) + ":D}", DELTAB[j]);
		Console::Write("\n\n");
		return;
	}

	void ShowResult() {
		int i, j;
		Console::Write("\nЗадача решена. Оптимальный план:\n");

		Console::WriteLine();
		for (i = 0; i < m; i++) {
			Console::Write("  ");
			for (j = 0; j < n; j++)
				Console::Write("{0,-" + (cellsize+1) + ":D}", X[i][j]);
			Console::WriteLine();
		}
		Console::WriteLine();

		int sum = 0;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				sum += CSTART[i][j] * X[i][j];
		Console::Write("Значение целевой функции: {0}\n", sum);
		return;
	}


protected:

	int m, n;
	int cellsize;
	int *A, *B, **C;
	int *A0, *B0, **C0;
	int **CSTART;
	int **X, **ZERO, *ROW0, *COL0;
	int *DELTAA, *DELTAB;
	bool adjust;
	int iteration;
	std::vector <elem> chain;

};

int main(array<System::String ^> ^args) {
	
	setlocale(LC_ALL, "Russian");
	Console::WriteLine("Венгерский метод для транспортной задачи\n");
	Hungarian *Data = new Hungarian();
	Data->Input();
	Data->Solve();
	Console::ReadKey(true);
	return 0;

}