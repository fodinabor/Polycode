/*
Copyright (C) 2014 by Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once
#include <Polycode.h>

#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#ifdef _WINDOWS
#include "CL/cl.hpp"
#elif defined(__APPLE__) && defined(__MACH__)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif //_WINODWS

namespace Polycode {

	/*
	*Friendly OpenCL Wrapper - You may not use unless you know what you do! Needs to be templated: 1. param: the typename you want to get back, 2. param: the length of the array to get back
	*/
	template <typename OpType, long length>
	class _PolyExport PolyCL : public PolyBase {
	public:

		/**
		* Constructor.
		* @param kernelSource A String containing the OpenCL C kernel source
		* @param kernelName A String containing the name of the OpenCL C kernel
		* @param arrays A vector of Arrays of any type which will be the parameters - the result should be stored in the last parameter, but is not stored in this array!
		* @param size a vector of the sizes of the arrays stored in arrays
		* @param result The OpenCL result Array is put in
		*/
		PolyCL(String kernelSource, String kernelName, std::vector< void* > arrays, std::vector< size_t > size, OpType *result) {
			try {
				if (init()) {
					buildProgram(kernelSource);
					createKernel(kernelName);
					createBuffers(arrays, size);
					executeKernel(result);
					cleanBuffers();
					success = true;
				}
				else {
					success = false;
					return;
				}
			}
			catch (cl::Error& err) {
				Logger::log("Error: %s\n", err.what());
				Logger::log("Error Code: %d\n", err.err());
				Logger::log("Build log: %d\n", programCL.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDeviceCL));
				success = false;
			}
		}

		/**
		* Standard constructor. Sets success whether init succeeded or not.
		*/
		PolyCL() {
			if (init()) {
				success = true;
			}
			else {
				success = false;
				return;
			}
		}

		/**
		* Sets the platforms, devices and creates the Context. 
		* @return if Platform and Device found true, otherwise false.
		*/
		bool init() {
			result = new OpType[length];
			if (!setPlatforms()) {
				Logger::log("OpenCL disabled!\n");
				return false;
			}
			else {
				if (!setDevicesFromPlatform(platformsCL[0])) {
					Logger::log("OpenCL disabled!\n");
					return false;
				}
				else {
					//setDevicesFromPlatform(platformsCL[0]);
					createContext();
					return true;
				};
				return true;
			}
		}

		/**
		* Sets the platforms.
		* @return if Platform found true, otherwise false.
		*/
		bool setPlatforms() {
			cl::Platform::get(&platformsCL);
			if (platformsCL.size() == 0) {
				Logger::log("OpenCL Platform fail\n");
				return false;
			}
			else {
				return true;
			}
		}

		/**
		* Sets devicesCL and defaultDeviceCL from a given platform. It first tries to use a GPU Device and if non has been found a CPU Device
		* @param platform A cl::Platform to use a Device from
		*/
		bool setDevicesFromPlatform(const cl::Platform& platform) {

			// Extract a vector of devices
			platform.getDevices(CL_DEVICE_TYPE_GPU, &devicesCL);
			if (devicesCL.size() == 0) {
				platform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCL);
				if (devicesCL.size() == 0) {
					Logger::log("No OpenCL Device\n");
					return false;
				}
				Logger::log("OpenCL Device: [CPU] ");
			}
			else {
				Logger::log("OpenCL Device: [GPU] ");
			}
			defaultDeviceCL = devicesCL[0];
			defaultDeviceCL.getInfo(CL_DEVICE_NAME, &deviceName);
			defaultDeviceCL.getInfo(CL_DEVICE_VENDOR, &vendorName);

			Logger::log("%s %s\n", vendorName.c_str(), deviceName.c_str());
			
			return true;
		}

		/**
		* Creates a Context and a CommandQueue
		*/
		void createContext() {
			contextCL = cl::Context(devicesCL);
			queueCL = cl::CommandQueue(contextCL, defaultDeviceCL);
		}

		/**
		* Builds the CL program from the given source
		* @param kernelSource A String containing the OpenCL C kernel source 
		*/
		void buildProgram(String kernelSource) {
			sourcesCL = cl::Program::Sources(1, std::make_pair(kernelSource.c_str(), kernelSource.length()));
			programCL = cl::Program(contextCL, sourcesCL);

			programCL.build(devicesCL);
		}

		/**
		* Creates a CL Kernel from the given name using the program created by buildProgram
		* @param kernelName A String containing the name of the OpenCL C kernel
		*/
		void createKernel(String kernelName) {
			kernelCL = cl::Kernel(programCL, kernelName.c_str());
		}

		/**
		* Creates and allocates the needed buffers.
		* @param arrays A vector of Arrays of any type which will be the parameters - the result should be stored in the last parameter, but is not stored in this array!
		* @param size a vector of the sizes of the arrays stored in arrays
		*/
		void createBuffers(std::vector< void* > arrays, std::vector< size_t > size) {
			for (int i = 0; i < arrays.size(); i++) {
				buffer.push_back(cl::Buffer(contextCL, CL_MEM_READ_WRITE, size[i]));
				queueCL.enqueueWriteBuffer(buffer[i], CL_TRUE, 0, size[i], arrays[i]);
				kernelCL.setArg(i, buffer[i]);
			}
			result_buffer = cl::Buffer(contextCL, CL_MEM_READ_WRITE, sizeof(OpType)*length);
			kernelCL.setArg(arrays.size(), result_buffer);
			
			//Logger::log("OpenCL Buffers: allocated and set\n");
		}

		/**
		* Finally executes the kernel and sets result
		* @param result The OpenCL result Array is put in
		*/
		void executeKernel(OpType* result) {
			queueCL.enqueueNDRangeKernel(kernelCL, cl::NullRange, cl::NDRange(length), cl::NullRange, NULL, &event);
			
			queueCL.finish();
			queueCL.enqueueReadBuffer(result_buffer, CL_TRUE, 0, sizeof(OpType)*length, (OpType*)result);
			this->result = result;

			//Logger::log("Result available now\n");
		}

		/**
		* Cleanes the buffers.
		*/
		void cleanBuffers() {
			buffer.~vector();
		}

		/**
		* Sets the defaultDeviceCL to the given device.
		* @param defaultDeviceCL A cl::Device to set the defaultDeviceCL from
		*/
		void setDefaultDevice(cl::Device defaultDeviceCL) {
			this->defaultDeviceCL = defaultDeviceCL;
		}

		/**
		* @return The default cl::Device
		*/
		cl::Device getDefaultDevice() {
			return defaultDeviceCL;
		}

		/**
		* @return The default device's name
		*/
		std::string getDefaultDeviceName() {
			return deviceName;
		}
		
		/**
		* @return The default device's vendor name
		*/
		std::string getDefaultDeviceVendorName() {
			return vendorName;
		}

		/**
		* @return The platforms as cl::vector< cl::Platform >
		*/
		cl::vector< cl::Platform > getPlatforms() {
			return platformsCL;
		}

		/**
		* @return The result
		*/
		OpType* getResult() {
			return result;
		}

		/**
		* @return Whether success is true or false
		*/
		bool getSuccess() {
			return success;
		}

	protected:
		cl::Device defaultDeviceCL;
		cl::Platform platformCL;
		cl::Kernel kernelCL;
		cl::Program programCL;
		cl::Program::Sources sourcesCL;
		cl::CommandQueue queueCL;
		cl::Context contextCL;
		cl::vector<cl::Device> devicesCL;
		cl::vector<cl::Platform> platformsCL;

		cl::vector<cl::Buffer> buffer;
		cl::Event event;
		cl::Buffer result_buffer;

		bool success;
		std::string vendorName;
		std::string deviceName;

		OpType* result;
	};
}
