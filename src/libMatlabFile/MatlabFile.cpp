/* The MIT License (MIT)

Copyright (c) 2006-2014 Otto Barnes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MatlabFile.h"

/**
 *
 */
MatlabFile::MatlabFile(const MatlabFileMode& mode) :
		mode(mode) {
}

/**
 *
 */
MatlabFile::~MatlabFile() {
	if (filename.compare("") != 0)
		close();
}

/**
 *
 */
long MatlabFile::open(const std::string &filename) {

	this->filename = filename;
	if (mode == READ) {

		// Open the file and the dataset.
		file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

//      LOG4CXX_INFO(logger, "Open for reading " << filename);

	} else if (mode == WRITE) {
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
void MatlabFile::close() {

	H5Fflush(file, H5F_SCOPE_LOCAL);
	H5Fclose(file);

	// Matlab header
	if (mode == WRITE) {
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
		if (fd != NULL) {
			fwrite(header, 1, HEADER_SIZE, fd);
			fclose(fd);
		} else {
			std::cout << "Failed to write MATLAB header." << std::endl;
		}
	}

	filename = "";
}

// -------------------------------------------------------------

template<> hid_t MatlabFile::getHdfType<char>(char w) {
	return H5T_NATIVE_CHAR;
}

template<> hid_t MatlabFile::getHdfType<signed char>(signed char w) {
	return H5T_NATIVE_SCHAR;
}

template<> hid_t MatlabFile::getHdfType<unsigned char>(unsigned char w) {
	return H5T_NATIVE_UCHAR;
}

template<> hid_t MatlabFile::getHdfType<short>(short w) {
	return H5T_NATIVE_SHORT;
}
template<> hid_t MatlabFile::getHdfType<unsigned short>(unsigned short w) {
	return H5T_NATIVE_USHORT;
}
template<> hid_t MatlabFile::getHdfType<int>(int w) {
	return H5T_NATIVE_INT;
}
template<> hid_t MatlabFile::getHdfType<unsigned>(unsigned w) {
	return H5T_NATIVE_UINT;
}
template<> hid_t MatlabFile::getHdfType<long>(long w) {
	return H5T_NATIVE_LONG;
}
template<> hid_t MatlabFile::getHdfType<unsigned long>(unsigned long w) {
	return H5T_NATIVE_ULONG;
}
template<> hid_t MatlabFile::getHdfType<long long>(long long w) {
	return H5T_NATIVE_LLONG;
}
template<> hid_t MatlabFile::getHdfType<unsigned long long>(
		unsigned long long w) {
	return H5T_NATIVE_ULLONG;
}
template<> hid_t MatlabFile::getHdfType<float>(float w) {
	return H5T_NATIVE_FLOAT;
}
template<> hid_t MatlabFile::getHdfType<double>(double w) {
	return H5T_NATIVE_DOUBLE;
}
template<> hid_t MatlabFile::getHdfType<long double>(long double w) {
	return H5T_NATIVE_LDOUBLE;
}
template<> hid_t MatlabFile::getHdfType<bool>(bool w) {
	return H5T_NATIVE_HBOOL;
}
