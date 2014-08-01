/*
 * MatlabFile.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: sio2
 */

#include "MatlabFile.hpp"
#include <ctime>
#include <unistd.h>
#include <iostream>

using namespace std;

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

		cout << "< Reading " << filename << "\n";

	} else if (mode == WRITE) {
		// Create a new file using H5F_ACC_TRUNC access
		cout << "> Writing " << filename << "\n";
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
long MatlabFile::writeArray(const std::string &variable_name,
		Float2dArray* array) {

	if (mode == READ) {
		cout << "    Can't write file in read mode" << endl;
		return -1;
	}

	if (array == NULL) {
		cout << "Can't access array passed to function." << endl;
		return -2;
	}

	hid_t datatype, dataspace;

	// Describe the size of the array and create the data space
	cout << "  - " << variable_name << " ";
	cout << "[" << array->x << "," << array->y << "] ...";
	hsize_t* dims = (hsize_t*)malloc(2*sizeof(hsize_t));
	dims[0] = array->y;
	dims[1] = array->x;
	dataspace = H5Screate_simple(2, dims, NULL);

	// Define datatype for the data in the file.

	// Create a new dataset within the file using defined dataspace and
	// datatype and default dataset creation properties.
	hid_t dataset = H5Dcreate(file, variable_name.c_str(), H5T_IEEE_F32LE, dataspace,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	// Write the data to the dataset using default transfer properties.

	status = H5Dwrite(dataset, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
			array->data);

	free(dims);

	// Check return code
	if (status == 0)
		cout << "done" << endl;
	else
		cout << "error " << status << endl;

	H5Dclose(dataset);
	H5Sclose(dataspace);

	return status;
}

/**
 *
 */
long MatlabFile::readArray(const std::string &variable_name,
		Float2dArray* &array) {

	if (mode == WRITE) {
		cout << "    Can't read file in write mode" << endl;
		return -1;
	}

	// Get dataspace and allocate memory for the read buffer as before.
	cout << "  - " << variable_name << "...";
	hid_t dataset = H5Dopen(file, variable_name.c_str(), H5P_DEFAULT);
	hid_t dataspace = H5Dget_space(dataset);

	hsize_t* dims = (hsize_t*)malloc(2*sizeof(hsize_t));
	int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);
	cout << "[" << dims[0] << "," << dims[1] << "]...";

	array = new Float2dArray(dims[0], dims[1]);

	// Read the data using the default properties.
	herr_t status = H5Dread(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
			H5P_DEFAULT, array->data);

	// Check return code
	if (status == 0)
		cout << "done" << endl;
	else
		cout << "error " << status << endl;

	free(dims);

	H5Sclose(dataspace);
	H5Dclose(dataset);

	return status;
}

/**
 *
 */
void MatlabFile::close() {

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
		rewind( fd );
		if (fd != NULL) {
			fwrite(header, 1, HEADER_SIZE, fd);
			fclose(fd);
		} else {
			cout << "Failed to write MATLAB header." << endl;
		}
	}

	filename = "";
}
