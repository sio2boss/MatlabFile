#ifndef MATLABFILE_HPP_
#define MATLABFILE_HPP_

#include <string>
#include <hdf5.h>
#include <stdio.h>      // printf, scanf, NULL
#include <stdlib.h>     // malloc, free, rand
#include <iostream>
#include <string.h>
#include <unistd.h>     // sleep

// Define types
// #include <vector_types.h>

// Only include if not using CUDA
struct float2
{
	float x, y;
};
struct double2
{
	double x, y;
};
struct int2
{
	int x, y;
};

enum MatlabFileMode
{
	READ, WRITE
};

/**
 * @class
 * @brief wrapper for HDF5 file format with Matlab header
 */
class MatlabFile
{

public:

	MatlabFile(const MatlabFileMode& mode);
	virtual ~MatlabFile();

	// Open/close
	long open(const std::string &filename);
	void close();

	// Values
	template<typename T> long readValue(const std::string &variable_name,
			T &data);
	template<typename T> long writeValue(const std::string &variable_name,
			const T value);

	// Arrays 1D, 2D, 3D
	template<typename T> long readArray(const std::string &variable_name,
			T* &data, unsigned int& x);

	template<typename T> long readArray(const std::string &variable_name,
			T* &data, unsigned int& x, unsigned int& y);

	template<typename T> long readArray(const std::string &variable_name,
			T* &data, unsigned int& x, unsigned int& y, unsigned int& z);

	template<typename T> long writeArray(const std::string &variable_name,
			const T* data, const unsigned int x = 1, const unsigned int y = 0,
			const unsigned int z = 0);

	// Arrays 1D, 2D, 3D for compound types
	template<typename T> long readCompoundArray(
			const std::string &variable_name, T* &data, unsigned int& x);

	template<typename T> long readCompoundArray(
			const std::string &variable_name, T* &data, unsigned int& x,
			unsigned int& y);

	template<typename T> long readCompoundArray(
			const std::string &variable_name, T* &data, unsigned int& x,
			unsigned int& y, unsigned int& z);

	template<typename T> long writeCompoundArray(
			const std::string &variable_name, const T* data,
			const unsigned int x = 1, const unsigned int y = 0,
			const unsigned int z = 0);

	// Helper for types
	template<typename T> hid_t getHdfType(T w);

protected:

	hid_t file;
	hid_t create_plist;
	herr_t status;

	MatlabFileMode mode;
	std::string filename;

	static const int HEADER_SIZE = 512;

};

/**
 *
 */
MatlabFile::MatlabFile(const MatlabFileMode& mode) :
		mode(mode)
{
}

/**
 *
 */
MatlabFile::~MatlabFile()
{
	if (filename.compare("") != 0)
		close();
}

/**
 *
 */
long MatlabFile::open(const std::string &filename)
{

	this->filename = filename;
	if (mode == READ)
	{

		// Open the file and the dataset.
		file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

//      LOG4CXX_INFO(logger, "Open for reading " << filename);

	}
	else if (mode == WRITE)
	{
		// Create a new file using H5F_ACC_TRUNC access
//      LOG4CXX_INFO(logger, "Open for writing " << filename);
		create_plist = H5Pcreate(H5P_FILE_CREATE);
		status = H5Pset_userblock(create_plist, HEADER_SIZE);
		file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, create_plist,
				H5P_DEFAULT);
	}

	return 0;
}

/**
 *
 */
void MatlabFile::close()
{

	H5Fflush(file, H5F_SCOPE_LOCAL);
	H5Fclose(file);

	// Matlab header
	if (mode == WRITE)
	{
		sleep(1);
		char header[HEADER_SIZE];
		memset(header, 0, HEADER_SIZE);
		sprintf(header, "MATLAB 7.3 MAT-file, .");
		header[124] = 0;
		header[125] = 2;
		header[126] = 'I';
		header[127] = 'M';

		FILE* fd = fopen(filename.c_str(), "rb+");
		rewind(fd);
		if (fd != NULL)
		{
			fwrite(header, 1, HEADER_SIZE, fd);
			fclose(fd);
		}
		else
		{
			std::cout << "Failed to write MATLAB header." << std::endl;
		}
	}

	filename = "";
}

// -------------------------------------------------------------

template<typename T> long MatlabFile::readValue(
		const std::string &variable_name, T &value)
{
	if (mode == WRITE)
		return -1;

	// Use readArray
	unsigned int x = 0;
	T* data = NULL;
	long status = readArray(variable_name, data, x);
	if (data == NULL)
		return status;

	// Return first value
	value = data[0];
	free(data);

	return status;
}

template<typename T> long MatlabFile::writeValue(
		const std::string &variable_name, const T value)
{

	if (mode == READ)
		return -1;

	// Use writeArray
	unsigned int x = 1;
	long status = writeArray(variable_name, &value, x);
	return status;
}

// -------------------------------------------------------------

template<typename T> long MatlabFile::readArray(
		const std::string &variable_name, T* &data, unsigned int& x)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(1 * sizeof(hsize_t));
	dims[0] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	data = (T *) malloc(x * sizeof(T));

	// Read the data using the default properties.
	T w;
	herr_t status = H5Dread(dataset, getHdfType(w), H5S_ALL, H5S_ALL,
			H5P_DEFAULT, data);

	free(dims);
	H5Sclose(dataspace);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::readArray(
		const std::string &variable_name, T* &data, unsigned int& x,
		unsigned int& y)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(2 * sizeof(hsize_t));
	dims[0] = 1;
	dims[1] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	y = dims[1];
	data = (T *) malloc(x * y * sizeof(T));

	// Read the data using the default properties.
	T w;
	herr_t status = H5Dread(dataset, getHdfType(w), H5S_ALL, H5S_ALL,
			H5P_DEFAULT, data);

	free(dims);
	H5Sclose(dataspace);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::readArray(
		const std::string &variable_name, T* &data, unsigned int& x,
		unsigned int& y, unsigned int &z)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(3 * sizeof(hsize_t));
	dims[0] = 1;
	dims[1] = 1;
	dims[2] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	y = dims[1];
	z = dims[2];
	data = (T *) malloc(x * y * z * sizeof(T));

	// Read the data using the default properties.
	T w;
	herr_t status = H5Dread(dataset, getHdfType(w), H5S_ALL, H5S_ALL,
			H5P_DEFAULT, data);

	free(dims);
	H5Sclose(dataspace);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::writeArray(
		const std::string &variable_name, const T* data, const unsigned int x,
		const unsigned int y, const unsigned int z)
{
	if (mode == READ)
		return -1;

	hid_t datatype, dataspace;

	// Describe the size of the array and create the data space
	hsize_t* dims = NULL;
	int num_dim = 0;
	if (y == 0 && z == 0)
	{
		num_dim = 1;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
	}
	else if (z == 0)
	{
		num_dim = 2;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
		dims[1] = y;
	}
	else
	{
		num_dim = 3;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
		dims[1] = y;
		dims[2] = z;
	}
	dims[0] = x;
	dataspace = H5Screate_simple(num_dim, dims, NULL);

	// Create a new dataset within the file using defined dataspace and
	// datatype and default dataset creation properties.
	T w;
	hid_t dataset = H5Dcreate(file, variable_name.c_str(), getHdfType(w),
			dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	// Write the data to the dataset using default transfer properties.
	status = H5Dwrite(dataset, getHdfType(w), H5S_ALL, H5S_ALL, H5P_DEFAULT,
			data);

	free(dims);
	status = H5Dclose(dataset);
	status = H5Sclose(dataspace);

	return status;
}

// -------------------------------------------------------------

template<typename T> long MatlabFile::readCompoundArray(
		const std::string &variable_name, T* &data, unsigned int& x)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(1 * sizeof(hsize_t));
	dims[0] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	data = (T *) malloc(x * sizeof(T));
	T w;

	// Define datatype for the data in the file
	hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(w.x) * 2);
	status = H5Tinsert(memtype, "real", 0, getHdfType(w.x));
	status = H5Tinsert(memtype, "imag", sizeof(getHdfType(w.x)),
			getHdfType(w.x));

	// Read the data using the default properties
	herr_t status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT,
			data);

	free(dims);
	H5Sclose(dataspace);
	H5Tclose(memtype);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::readCompoundArray(
		const std::string &variable_name, T* &data, unsigned int& x,
		unsigned int& y)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(2 * sizeof(hsize_t));
	dims[0] = 1;
	dims[1] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	y = dims[1];
	data = (T *) malloc(x * y * sizeof(T));
	T w;

	// Define datatype for the data in the file
	hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(w.x) * 2);
	status = H5Tinsert(memtype, "real", 0, getHdfType(w.x));
	status = H5Tinsert(memtype, "imag", sizeof(getHdfType(w.x)),
			getHdfType(w.x));

	// Read the data using the default properties
	herr_t status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT,
			data);

	free(dims);
	H5Sclose(dataspace);
	H5Tclose(memtype);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::readCompoundArray(
		const std::string &variable_name, T* &data, unsigned int& x,
		unsigned int& y, unsigned int &z)
{
	if (mode == WRITE)
		return -1;

	// Get dataspace and allocate memory for the read buffer as before.
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*) malloc(3 * sizeof(hsize_t));
	dims[0] = 1;
	dims[1] = 1;
	dims[2] = 1;
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);

	x = dims[0];
	y = dims[1];
	z = dims[2];
	data = (T *) malloc(x * y * z * sizeof(T));
	T w;

	// Define datatype for the data in the file
	hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(w.x) * 2);
	status = H5Tinsert(memtype, "real", 0, getHdfType(w.x));
	status = H5Tinsert(memtype, "imag", sizeof(getHdfType(w.x)),
			getHdfType(w.x));

	// Read the data using the default properties
	herr_t status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT,
			data);

	free(dims);
	H5Sclose(dataspace);
	H5Tclose(memtype);
	H5Dclose(dataset);

	return status;
}

template<typename T> long MatlabFile::writeCompoundArray(
		const std::string &variable_name, const T* data, const unsigned int x,
		const unsigned int y, const unsigned int z)
{
	if (mode == READ)
		return -1;

	hid_t datatype, dataspace;

	// Describe the size of the array and create the data space
	hsize_t* dims = NULL;
	int num_dim = 0;
	if (y == 0 && z == 0)
	{
		num_dim = 1;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
	}
	else if (z == 0)
	{
		num_dim = 2;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
		dims[1] = y;
	}
	else
	{
		num_dim = 3;
		dims = (hsize_t*) malloc(num_dim * sizeof(hsize_t));
		dims[1] = y;
		dims[2] = z;
	}
	dims[0] = x;
	dataspace = H5Screate_simple(num_dim, dims, NULL);
	T w;

	// Define datatype for the data in the file
	hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(getHdfType(w.x)) * 2);
	status = H5Tinsert(memtype, "real", 0, getHdfType(w.x));
	status = H5Tinsert(memtype, "imag", sizeof(getHdfType(w.x)),
			getHdfType(w.x));

	// Create a new dataset within the file using defined dataspace and
	// datatype and default dataset creation properties.
	hid_t dataset = H5Dcreate(file, variable_name.c_str(), memtype, dataspace,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	// Write the data to the dataset using default transfer properties.
	status = H5Dwrite(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	free(dims);
	status = H5Dclose(dataset);
	status = H5Tclose(memtype);
	status = H5Sclose(dataspace);

	return status;
}

// -------------------------------------------------------------

template<> hid_t MatlabFile::getHdfType<char>(char w)
{
	return H5T_NATIVE_CHAR;
}

template<> hid_t MatlabFile::getHdfType<signed char>(signed char w)
{
	return H5T_NATIVE_SCHAR;
}

template<> hid_t MatlabFile::getHdfType<unsigned char>(unsigned char w)
{
	return H5T_NATIVE_UCHAR;
}

template<> hid_t MatlabFile::getHdfType<short>(short w)
{
	return H5T_NATIVE_SHORT;
}
template<> hid_t MatlabFile::getHdfType<unsigned short>(unsigned short w)
{
	return H5T_NATIVE_USHORT;
}
template<> hid_t MatlabFile::getHdfType<int>(int w)
{
	return H5T_NATIVE_INT;
}
template<> hid_t MatlabFile::getHdfType<unsigned>(unsigned w)
{
	return H5T_NATIVE_UINT;
}
template<> hid_t MatlabFile::getHdfType<long>(long w)
{
	return H5T_NATIVE_LONG;
}
template<> hid_t MatlabFile::getHdfType<unsigned long>(unsigned long w)
{
	return H5T_NATIVE_ULONG;
}
template<> hid_t MatlabFile::getHdfType<long long>(long long w)
{
	return H5T_NATIVE_LLONG;
}
template<> hid_t MatlabFile::getHdfType<unsigned long long>(
		unsigned long long w)
{
	return H5T_NATIVE_ULLONG;
}
template<> hid_t MatlabFile::getHdfType<float>(float w)
{
	return H5T_NATIVE_FLOAT;
}
template<> hid_t MatlabFile::getHdfType<double>(double w)
{
	return H5T_NATIVE_DOUBLE;
}
template<> hid_t MatlabFile::getHdfType<long double>(long double w)
{
	return H5T_NATIVE_LDOUBLE;
}
template<> hid_t MatlabFile::getHdfType<bool>(bool w)
{
	return H5T_NATIVE_HBOOL;
}

#endif /* MATLABFILE_HPP_ */
