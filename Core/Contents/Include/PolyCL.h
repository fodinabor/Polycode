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

#ifndef __NO_STD_VECTOR
#define __NO_STD_VECTOR
#endif //__NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#ifdef _WINDOWS
#include "CL/cl.hpp"
#elif defined(__APPLE__) && defined(__MACH__)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

namespace Polycode {

	template <class OpType, long length>
	class _PolyExport PolyCL : public PolyBase {
	public:

		PolyCL(String kernelSource, String kernelName, std::vector< OpType > arrays, OpType *result) {
			try {
				init();
				buildProgram(kernelSource);
				createKernel(kernelName);
				createBuffers(arrays);
				executeKernel();
				cleanBuffers();
				success = true;
			}
			catch (cl::Error& err) {
				Logger::log("Error: %s\n", err.what());
				Logger::log("Error Code: %d\n", err.err());
				Logger::log("Build log: %d\n", programCL.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devicesCL[0]));

				success = false;
			}
		}

		PolyCL() {
			init();
		}

		void init() {
			if (!setPlatforms()) {
				Logger::log("OpenCL disabled!\n");
				exit(EXIT_FAILURE);
			}
			else {
				setDevicesFromPlatform(platformsCL[0]);
				createContext();
			}
		}

		bool setPlatforms() {
			cl::Platform::get(&platformsCL);
			if (platformsCL.size() == 0) {
				Logger::log("Platform fail\n");
				return false;
			}
			else {
				return true;
			}
		}

		bool setDevicesFromPlatform(const cl::Platform& platform) {

			// Extract a vector of devices
			platform.getDevices(CL_DEVICE_TYPE_GPU, &devicesCL);
			if (devicesCL.size() == 0) {
				platform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCL);
				if (devicesCL.size() == 0) {
					Logger::log("no Device");
					return false;
				}
				Logger::log("OpenCL Device Type: CPU\n");
			}
			else {
				Logger::log("OpenCL Device Type: GPU\n");
			}
			defaultDeviceCL = devicesCL[0];
			setComputeUnits();
			defaultDeviceCL.getInfo(CL_DEVICE_NAME, &deviceName);
			defaultDeviceCL.getInfo(CL_DEVICE_VENDOR, &vendorName);

			Logger::log("OpenCL Device found: %s %s\n", vendorName.c_str(), deviceName.c_str());
			Logger::log("Units: %d\n", computeUnits);

			return true;

		}

		void createContext() {
			contextCL = cl::Context(devicesCL);
			queueCL = cl::CommandQueue(contextCL, defaultDeviceCL);
		}

		void buildProgram(String kernelSource) {
			sourcesCL = cl::Program::Sources(1, std::make_pair(kernelSource.c_str(), kernelSource.length()));
			programCL = cl::Program(contextCL, sourcesCL);

			programCL.build(devicesCL);
		}

		void createKernel(String kernelName) {
			kernelCL = cl::Kernel(programCL, kernelName.c_str());
		}

		void createBuffers(std::vector< OpType > arrays) {
			for (int i = 0; i < arrays.size(); i++) {
				Logger::log("Allocating Buffer %d\n", i);
				for (int k = 0; k < length; k++) {
					Logger::log("CoA %d\n", arrays[i][k]);
				}
				buffer.push_back(cl::Buffer(contextCL, CL_MEM_READ_WRITE, sizeof(OpType)*length));
				queueCL.enqueueWriteBuffer(buffer[i], CL_TRUE, 0, sizeof(OpType)*length, arrays[i]);
				Logger::log("Buffer %d allocated\n", i);
				kernelCL.setArg(i, buffer[i]);
				Logger::log("Buffer %d set\n", i);
			}
			result_buffer = cl::Buffer(contextCL, CL_MEM_WRITE_ONLY, length*sizeof(OpType));
			kernelCL.setArg(arrays.size(), result_buffer);
		}

		void executeKernel() {
			Logger::log("test0\n");
			queueCL.enqueueNDRangeKernel(kernelCL, cl::NullRange, cl::NDRange(length), cl::NullRange, NULL, &event);
			Logger::log("test1\n");
			event.wait();
			Logger::log("test2\n");
			queueCL.enqueueReadBuffer(result_buffer, CL_TRUE, 0, sizeof(OpType)*length, (OpType)result);
			for (int i = 0; i < length; i++) {
				Logger::log("Result: %d", result[i]);
			}
		}

		void cleanBuffers() {
			buffer.~vector();
		}

		void setDefaultDevice(cl::Device newdefDevice) {
			defaultDeviceCL = newdefDevice;
		}

		void setComputeUnits() {
			defaultDeviceCL.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &computeUnits);
		}

		int getComputeUnits() {
			return computeUnits;
		}

		cl::Device getDefaultDevice() {
			return defaultDeviceCL;
		}

		cl::vector< cl::Platform > getPlatforms() {
			return platformsCL;
		}

		OpType getResult() {
			return result;
		}

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

		cl::Buffer result_buffer;
		cl::vector<cl::Buffer> buffer;
		cl::Event event;

		bool success;
		std::string vendorName;
		std::string deviceName;
		int computeUnits;
		OpType result;
	};
}
