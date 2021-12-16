#include "stdafx.h"
#include "MyPCA.h"



#include "tnt_array1d.h"

#include "jama_eig.h"


#include <fstream>
#include <iomanip>
#include <assert.h>
#include <sstream>
#include <Windows.h>
//#include <QFile>

using namespace std;
using namespace TNT;
using namespace JAMA;
namespace PCA {
	bool debug = false;

	//按第一行展开计算|A|
	double getA(const Array2D<double>& arcs, int n)
	{
		if (n == 1)
		{
			return arcs[0][0];
		}
		double ans = 0;
		Array2D<double> temp(n, n);
		int i, j, k;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n - 1; j++)
			{
				for (k = 0; k < n - 1; k++)
				{
					temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

				}
			}
			double t = getA(temp, n - 1);
			if (i % 2 == 0)
			{
				ans += arcs[0][i] * t;
			}
			else
			{
				ans -= arcs[0][i] * t;
			}
		}
		return ans;
	}

	//计算每一行每一列的每个元素所对应的余子式，组成A*
	void  getAStart(const Array2D<double>& arcs, int n, Array2D<double>& ans)
	{
		if (n == 1)
		{
			ans[0][0] = 1;
			return;
		}
		int i, j, k, t;
		Array2D<double> temp(n, n);
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				for (k = 0; k < n - 1; k++)
				{
					for (t = 0; t < n - 1; t++)
					{
						temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
					}
				}


				ans[j][i] = getA(temp, n - 1);
				if ((i + j) % 2 == 1)
				{
					ans[j][i] = -ans[j][i];
				}
			}
		}
	}

	// 得到给定矩阵src的逆矩阵保存到des中。
	bool GetMatrixInverse(const Array2D<double>& src, int n, Array2D<double>& des)
	{
		double flag = getA(src, n);

		Array2D<double> t(n, n);
		if (flag == 0)
		{
			return false;
		}
		else
		{
			getAStart(src, n, t);
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					des[i][j] = t[i][j] / flag;
				}

			}
		}


		return true;

	}

	// 自己实现的方法，使用初等变换
	void GetMatrixInverse_2(const Array2D<double>& src, int n, Array2D<double>& des) {
		// 0.initialized des to E
		Array2D<double> srcTemp(n, n);
		for (size_t i = 0; i < n; i++)
		{
			for (size_t j = 0; j < n; j++)
			{
				if (i == j) des[i][j] = 1;
				else des[i][j] = 0;

				srcTemp[i][j] = src[i][j];
			}
		}

		// 1.transform
		for (size_t i = 0; i < n; i++)
		{
			// 2.up down minus
			for (size_t k = 0; k < i; k++)		// each line before this line
			{
				double dbScale = srcTemp[i][k];
				for (size_t j = 0; j < n; j++)
				{
					srcTemp[i][j] -= (srcTemp[k][j] * dbScale);
					des[i][j] -= (des[k][j] * dbScale);
				}
			}
			// 1.normalize
			double dbScale = srcTemp[i][i];
			for (size_t j = 0; j < n; j++)
			{
				srcTemp[i][j] /= dbScale;
				des[i][j] /= dbScale;
			}
		}
		// 2.bottom up minus
		for (int i = n - 2; i >= 0; i--)
		{
			for (size_t k = n - 1; k > i; k--)
			{
				double dbScale = srcTemp[i][k];
				for (size_t j = 0; j < n; j++)
				{
					srcTemp[i][j] -= (srcTemp[k][j] * dbScale);
					des[i][j] -= (des[k][j] * dbScale);
				}
			}
		}
	}

	template < class T>
	void convert_from_string(T& value, const string& s)
	{
		stringstream ss(s);
		ss >> value;
	}

	void load_data_from_file(Array2D<double>& d, char*& file_path) {
		ifstream in(file_path);
		string line;
		int r = 0;

		if (in.is_open()) {
			while (in.good()) {
				int col = 0;
				getline(in, line);
				if (line.empty()) continue;

				size_t start_pos = 0;
				char space = ' ';
				while (true) {
					size_t pos = line.find(space, start_pos);
					string data(line.substr(start_pos, pos - start_pos));
					if (!data.empty()) {
						double v = 0;
						convert_from_string(v, data);

						if (debug)
							cout << "value: " << v << endl;
						d[r][col] = v;
					}

					if ((int)pos != -1) {
						start_pos = pos + 1;
					}
					else {
						break;
					}
					col += 1;
				}
				r += 1;
			}
			in.close();
		}
	}

	void adjust_data(Array2D<double>& d, Array1D<double>& means) {
		for (int i = 0; i < d.dim2(); ++i) {
			double mean = 0;
			for (int j = 0; j < d.dim1(); ++j) {
				mean += d[j][i];
			}

			mean /= d.dim1();

			// store the mean
			means[i] = mean;

			// subtract the mean
			for (int j = 0; j < d.dim1(); ++j) {
				d[j][i] -= mean;
			}
		}
	}

	double compute_covariance(const Array2D<double>& d, int i, int j) {
		double cov = 0;
		for (int k = 0; k < d.dim1(); ++k) {
			cov += d[k][i] * d[k][j];
		}

		return cov / (d.dim1() - 1);
	}

	void compute_covariance_matrix(const Array2D<double> & d, Array2D<double> & covar_matrix) {
		int dim = d.dim2();
		assert(dim == covar_matrix.dim1());
		assert(dim == covar_matrix.dim2());
		for (int i = 0; i < dim; ++i) {
			for (int j = i; j < dim; ++j) {
				covar_matrix[i][j] = compute_covariance(d, i, j);
			}
		}


		// fill the Left triangular matrix
		for (int i = 1; i < dim; i++) {
			for (int j = 0; j < i; ++j) {
				covar_matrix[i][j] = covar_matrix[j][i];
			}
		}

	}

	// Calculate the eigenvectors and eigenvalues of the covariance
	// matrix
	void eigen(const Array2D<double> & covar_matrix, Array2D<double>& eigenvector, Array2D<double>& eigenvalue) {
		Eigenvalue<double> eig(covar_matrix);
		eig.getV(eigenvector);
		eig.getD(eigenvalue);
	}


	void transpose(const Array2D<double>& src, Array2D<double>& target) {
		for (int i = 0; i < src.dim1(); ++i) {
			for (int j = 0; j < src.dim2(); ++j) {
				target[j][i] = src[i][j];
			}
		}
	}

	// z = x * y
	void multiply(const Array2D<double>& x, const Array2D<double>& y, Array2D<double>& z) {
		assert(x.dim2() == y.dim1());
		for (int i = 0; i < x.dim1(); ++i) {
			for (int j = 0; j < y.dim2(); ++j) {
				double sum = 0;
				int d = y.dim1();
				for (int k = 0; k < d; k++) {
					sum += x[i][k] * y[k][j];
				}
				z[i][j] = sum;
			}
		}
	}
}

using namespace PCA;

MyPCA::MyPCA():_arrDataMean(0)
, _pR(0)
, _pCovar_matrix(0)
, _pEigenvalue(0)
, _pEigenvector(0)
, _pD(0)
{
}

MyPCA::~MyPCA()
{
	if (_arrDataMean) delete[] _arrDataMean;
	if (_pR) delete _pR;
	if (_pCovar_matrix) delete _pCovar_matrix;
	if (_pEigenvalue) delete _pEigenvalue;
	if (_pEigenvector) delete _pEigenvector;
	if (_pD) delete _pD;
}


void MyPCA::DoPCA(const double* arrInput, double* arrOutput, int n, int mI,int mO, bool bNewData) {
	// -1.release the resource, and reallocate
	if (_arrDataMean) delete[] _arrDataMean;
	if (_pR) delete _pR;
	if (_pCovar_matrix) delete _pCovar_matrix;
	if (_pEigenvalue) delete _pEigenvalue;
	if (_pEigenvector) delete _pEigenvector;
	_nRow = n;
	_nCol = mI;
	_pR = new Array2D<double>(_nCol, _nCol);
	_pCovar_matrix = new Array2D<double>(_nCol, _nCol);
	_pEigenvalue = new Array2D<double>(_nCol, _nCol);
	_pEigenvector = new Array2D<double>(_nCol, _nCol);
	_pD = new Array2D<double>(_nRow, _nCol);


	_arrDataMean = new double[_nCol];
	int nPCALen = mO;

	// 0.calculate mean
	for (size_t i = 0; i < _nCol; i++)
	{
		double fMean = 0;
		for (size_t j = 0; j < _nRow; j++)
		{
			fMean += arrInput[j*_nCol + i];
		}
		_arrDataMean[i] = fMean / _nRow;
	}
	// 1.build the data struture, minus mean
	for (int i = 0; i < _nRow; i++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			(*_pD)[i][j] = arrInput[i*_nCol + j] - _arrDataMean[j];
		}
	}
	if (true)
	{
		calculateMatrices();
	}
	else {
		// old codes, write the matrix
		cout << "to calculate matrix" << endl;
		// 2.PCA
		if (bNewData)
		{
			calculateMatrices();
			writeMatrices();
		}
		else {
			readMatrices();
		}
		cout << "finished calculate matrix" << endl;
	}

	// 3.projection
	Array2D<double> final_data(_nRow, nPCALen);							// projected data
	Array2D<double> projection_matrix(_nCol, nPCALen);					// pca of eigenvector members
	for (int i = _nCol - 1, ii = 0; ii < nPCALen; i--, ii++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			projection_matrix[j][ii] = (*_pEigenvector)[j][i];
		}
	}
	multiply(*_pD, projection_matrix, final_data);							// project the data


	if (true) 
	{
		double dbSum = 0;
		for (size_t i = 0; i < _nCol; i++)
		{
			//cout << (*_pEigenvalue)[i][i] << endl;
			dbSum += (*_pEigenvalue)[i][i];
		}
		//for (size_t i = _nCol-1; i > _nCol-50; i--)
		cout << "===eigen value:" << endl;
		for (int i = _nCol - 1; i >= 0; i--)
		{
			cout << (*_pEigenvalue)[i][i] / dbSum << endl;
		}
	}


	cout << "===PCs:" << endl;
	for (size_t i = 0; i < _nRow; i++)
	{
		for (size_t j = 0; j < mO; j++)
		{
			arrOutput[i*mO + j] = final_data[i][j];// *(*_pEigenvalue)[j][j];
			if(j<3) cout << arrOutput[i*mO + j] << "  ";
		}
		cout << endl;
	}
}


void MyPCA::DoEOF(const double* arrInput, double* arrModes, double* arrFactors, int n, int mI, int mO) {
	// -1.release the resource, and reallocate
	if (_arrDataMean) delete[] _arrDataMean;
	if (_pCovar_matrix) delete _pCovar_matrix;
	if (_pEigenvalue) delete _pEigenvalue;
	if (_pEigenvector) delete _pEigenvector;
	_nRow = n;
	_nCol = mI;
	_pCovar_matrix = new Array2D<double>(_nCol, _nCol);
	_pEigenvalue = new Array2D<double>(_nCol, _nCol);
	_pEigenvector = new Array2D<double>(_nCol, _nCol);
	_pD = new Array2D<double>(_nRow, _nCol);


	_arrDataMean = new double[_nCol];
	int nPCALen = mO;

	// 0.calculate mean
	for (size_t i = 0; i < _nCol; i++)
	{
		double fMean = 0;
		for (size_t j = 0; j < _nRow; j++)
		{
			fMean += arrInput[j*_nCol + i];
		}
		_arrDataMean[i] = fMean / _nRow;
	}
	// 1.build the data struture, minus mean
	for (int i = 0; i < _nRow; i++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			(*_pD)[i][j] = arrInput[i*_nCol + j] - _arrDataMean[j];
		}
	}

	// 1.calculate covariance matrix
	compute_covariance_matrix(*_pD, *_pCovar_matrix);


	// 2.get the eigenvectors and eigenvalues
	eigen(*_pCovar_matrix, *_pEigenvector, *_pEigenvalue);

	// 3.projection
	Array2D<double> final_data(_nRow, nPCALen);							// projected data
	Array2D<double> projection_matrix(_nCol, nPCALen);					// pca of eigenvector members
	for (int i = _nCol - 1, ii = 0; ii < nPCALen; i--, ii++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			projection_matrix[j][ii] = (*_pEigenvector)[j][i];
		}
	}
	multiply(*_pD, projection_matrix, final_data);							// project the data


	if (true)
	{
		double dbSum = 0;
		for (int i = 0; i < _nCol; i++)
		{
			//cout << (*_pEigenvalue)[i][i] << endl;
			dbSum += (*_pEigenvalue)[i][i];
		}
		//for (size_t i = _nCol-1; i > _nCol-50; i--)
		for (int i = _nCol - 1; i >= 0; i--)
		{
			cout << (*_pEigenvalue)[i][i] / dbSum << endl;
		}
	}

	for (int j = 0; j < mO; j++)
	{
		for (int i = 0; i < _nCol; i++)
		{
			arrFactors[i*mO + j] = projection_matrix[i][j];
			cout << arrFactors[i*mO + j] << endl;
		}
		cout << endl;
	}
	for (int i = 0; i < _nRow; i++)
	{
		for (int j = 0; j < mO; j++)
		{
			arrModes[i*mO + j] = final_data[i][j];
			//			cout << arrOutput[i*mO + j] << endl;
		}
	}
}
void MyPCA::TestPCARecover(double* arrInput, int n, int mI, int mO, bool bNewData) {
	// -1.release the resource, and reallocate
	if (_arrDataMean) delete[] _arrDataMean;
	if (_pR) delete _pR;
	if (_pCovar_matrix) delete _pCovar_matrix;
	if (_pEigenvalue) delete _pEigenvalue;
	if (_pEigenvector) delete _pEigenvector;
	_nRow = n;
	_nCol = mI;
	_pR = new Array2D<double>(_nCol, _nCol);
	_pCovar_matrix = new Array2D<double>(_nCol, _nCol);
	_pEigenvalue = new Array2D<double>(_nCol, _nCol);
	_pEigenvector = new Array2D<double>(_nCol, _nCol);
	_pD = new Array2D<double>(_nRow, _nCol);


	_arrDataMean = new double[_nCol];
	int nPCALen = mO;

	// 0.calculate mean
	for (size_t i = 0; i < _nCol; i++)
	{
		double fMean = 0;
		for (size_t j = 0; j < _nRow; j++)
		{
			fMean += arrInput[j*_nCol + i];
		}
		_arrDataMean[i] = fMean / _nRow;
	}
	// 1.build the data struture, minus mean
	for (int i = 0; i < _nRow; i++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			(*_pD)[i][j] = arrInput[i*_nCol + j] - _arrDataMean[j];
		}
	}
	if (false)
	{
		calculateMatrices();
	}
	else {
		// old codes, write the matrix
		cout << "to calculate matrix" << endl;
		// 2.PCA
		if (bNewData)
		{
			calculateMatrices();
			writeMatrices();
		}
		else {
			readMatrices();
		}
		cout << "finished calculate matrix" << endl;
	}

	// 3.projection
	Array2D<double> final_data(_nRow, nPCALen);							// projected data
	Array2D<double> pca_eigenvector(nPCALen, _nCol);					// pca of eigenvector members
	for (size_t i = _nCol - nPCALen, ii = 0; i < _nCol; i++, ii++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			pca_eigenvector[ii][j] = (*_pEigenvector)[i][j];
		}
	}
	Array2D<double> transpose_pca_eigenvector(_nCol, nPCALen);			// transposed pca of eigenvector members
	transpose(pca_eigenvector, transpose_pca_eigenvector);
	multiply(*_pD, transpose_pca_eigenvector, final_data);							// project the data

	for (size_t j = 0; j < _nRow; j++) {
		cout << final_data[j][nPCALen - 1] << endl;
	}
	return;

	for (size_t i = nPCALen-50; i < nPCALen; i++)
	{
		for (size_t j = 0; j < _nRow; j++)
		{
			final_data[j][i] = 0;
		}

	}

	// recover

	Array2D<double> back_data(_nRow, _nCol);
	Array2D<double> transposed_r(_nCol, _nCol);
	transpose(*_pR, transposed_r);
	
	multiply(final_data, transposed_r, back_data);


	for (size_t i = 0; i < _nRow; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			arrInput[i*_nCol + j] = _arrDataMean[j] + back_data[i][j];
		}
	}
}

void MyPCA::Recover(double* arrInput, double* arrOutput, int nRow, int nCol) {
	Array2D<double> back_data(nRow, _nCol);

	Array2D<double> pca_data(nRow, _nCol);
	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			if(j<(_nCol-nCol)) pca_data[i][j] = 0;
			else pca_data[i][j] = arrInput[i*nCol + j- (_nCol - nCol)];
		}
	}

	multiply(pca_data, *_pR, back_data);


	for (size_t i = 0; i < nRow; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			arrOutput[i*_nCol + j] = _arrDataMean[j] + back_data[i][j];
		}
	}
}

void MyPCA::Recover(double* arrInput, double* arrOutput, int nCol) {
	Array2D<double> back_data(1, _nCol);

	Array2D<double> pca_data(1, _nCol);

	for (int j = 0; j < _nCol; j++)
	{
		if (j < (_nCol - nCol)) pca_data[0][j] = 0;
		else pca_data[0][j] = arrInput[j - (_nCol - nCol)];
	}

	multiply(pca_data, *_pR, back_data);

	for (size_t j = 0; j < _nCol; j++)
	{
		arrOutput[j] = _arrDataMean[j] + back_data[0][j];
	}
}

void MyPCA::readMatrices() {
	/*
	// read binary file
	QFile file("pca_binary.bin");
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	char* temp = new char[file.size()];
	file.read(temp, file.size());
	double* f = (double*)temp;
	int x = file.size();


	int t0 = GetTickCount();
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			(*_pCovar_matrix)[i][j] = *f++;
		}
	}
	int t = GetTickCount();
	cout << "reading covar_matrix finished:\t" << t - t0 << endl;
	t0 = t;
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			(*_pEigenvector)[i][j] = *f++;
		}
	}
	t = GetTickCount();
	cout << "reading eigenvector finished:\t" << t - t0 << endl;
	t0 = t;
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			(*_pEigenvalue)[i][j] = *f++;
		}
	}
	t = GetTickCount();
	cout << "reading eigenvalue finished:\t" << t - t0 << endl;
	t0 = t;
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			(*_pR)[i][j] = *f++;
		}
	}
	t = GetTickCount();
	cout << "reading R finished:\t" << t - t0 << endl;
	t0 = t;
	*/
}

void MyPCA::calculateMatrices() {
	int t0 = GetTickCount();

	// 1.calculate covariance matrix
	compute_covariance_matrix(*_pD, *_pCovar_matrix);

	int t = GetTickCount();
	cout << "covariance finished:\t" << t - t0 << endl;
	t0 = t;

	// 2.get the eigenvectors and eigenvalues
	eigen(*_pCovar_matrix, *_pEigenvector, *_pEigenvalue);

	t = GetTickCount();
	cout << "eigen finished:\t" << t - t0 << endl;
	t0 = t;

	//GetMatrixInverse_2(*_pEigenvector, _nCol, *_pR);

	/*
	cout << "nCol:" << _nCol << endl;
	for (int i = 0; i < _nCol; i++)
	{
		for (int j = 0; j < _nCol; j++)
		{
			cout << "hehe: " << (*_pR)[i][j] << endl;
		}
	}
	*/

	t = GetTickCount();
	cout << "R finished:\t" << t - t0 << endl;
	t0 = t;
}

void MyPCA::writeMatrices() {
	// write the calculated data to file
	int nLen = _nCol*_nCol * 4;
	double* buf = new double[nLen];
	double* pBuf = buf;

	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			*pBuf++ = (*_pCovar_matrix)[i][j];
		}
	}
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			*pBuf++ = (*_pEigenvector)[i][j];
		}
	}
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			*pBuf++ = (*_pEigenvalue)[i][j];
		}
	}
	for (size_t i = 0; i < _nCol; i++)
	{
		for (size_t j = 0; j < _nCol; j++)
		{
			*pBuf++ = (*_pR)[i][j];
		}
	}

	// output file
	ofstream output_binary;
	output_binary.open("pca_binary.bin", ios::out | ios::binary);
	output_binary.write((char*)buf, nLen*sizeof(double));
	delete[] buf;
}

void MyPCA::generateEllipse(std::vector<DPoint3>& points, int* arrGridLabel, int label, int nWidth, int nHeight, double dbDis) {
	// calculate the confidence eclipse

	int row = 0;
	int col = 2;
	double xMean = 0;
	double yMean = 0;
	for (size_t i = 0; i < nHeight; i++)
	{
		for (size_t j = 0; j < nWidth; j++)
		{
			if (arrGridLabel[i*nWidth + j] == label) {
				xMean += j;
				yMean += i;
				row++;
			}
		}
	}
	xMean /= row;
	yMean /= row;

	Array2D<double> d(row, col);
	int nRowIndex = 0;
	for (size_t i = 0; i < nHeight; i++)
	{
		for (size_t j = 0; j < nWidth; j++)
		{
			if (arrGridLabel[i*nWidth + j]==label) {
				d[nRowIndex][0] = j - xMean;
				d[nRowIndex][1] = i - yMean;
				nRowIndex++;
			}
		}
	}
	// 2.compute covariance matrix
	Array2D<double> covar_matrix(col, col);
	compute_covariance_matrix(d, covar_matrix);
	Array2D<double> covar_matrix_r(col, col);

	// 3.compute the reverse matrix of covariance
	GetMatrixInverse(covar_matrix, col, covar_matrix_r);

	// 4.generate result points
	Array2D<double> m0(1, 2);
	Array2D<double> m1(2, 1);
	Array2D<double> m(1, 2);
	Array2D<double> mr(1, 1);
	for (double i = 0; i < nHeight; i++)
	{
		for (double j = 0; j < nWidth; j++)
		{
			m0[0][0] = m1[0][0] = j - xMean;
			m0[0][1] = m1[1][0] = i - yMean;
			multiply(m0, covar_matrix_r, m);
			multiply(m, m1, mr);
			double alpha = sqrt(mr[0][0]);
			if (alpha<dbDis)
			{
				points.push_back(DPoint3(j, i, 0));
			}
		}
	}
}

