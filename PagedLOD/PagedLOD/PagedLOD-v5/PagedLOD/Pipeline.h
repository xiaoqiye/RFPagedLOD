#pragma once
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>

namespace hivePagedLOD
{
	template <class TInputData, class TInputBuffer, class TOutputData, class TOutputBuffer>
	class IPipeline
	{
	public:
		IPipeline() {}
		virtual ~IPipeline() {}

		//*****************************************************************************************
		//FUNCTION: std::pair<bool, int> = <whether push success, number of data pushed to output buffer>
		std::pair<bool, int> tryPush(const TInputData& vInput)
		{
			_ASSERTE(m_pInputBuffer);
			if (!_push2InputBuffer(vInput)) return std::make_pair(false, 0);
			if (!m_IsAysnConvertData)
			{
				_ASSERTE(m_pOutputBuffer);
				TInputData Input;
				TOutputData Output;
				int NumDataMovedOutputBuffer = 0;
				while (!m_pInputBuffer->empty())
				{
					Input = __getFirstDataInInputBufferV();
					Output = __convertDataV(Input);
					if (_push2OutputBuffer(Output))
					{
						__popFirstDataInInputBufferV();
						NumDataMovedOutputBuffer++;
					}
					else
						break;
				}
				return std::make_pair(true, NumDataMovedOutputBuffer);
			}
			else
			{
				return std::make_pair(true, 0);
			}
		}

		//*****************************************************************************************
		//FUNCTION:
		bool tryPop(TOutputData& voOutput)
		{
			_ASSERTE(m_pOutputBuffer);
			if (!__popFromOutputBufferV(voOutput)) return false;

			std::lock_guard<std::mutex> Lock(m_OutputBufferWritableMutex);
			m_IsOutputBufferWritable = true;
			++m_PerfStatistic.NumDataPopFromOutputBuffer;
			m_OutputBufferWritableSignal.notify_one();
			return true;
		}

		//*****************************************************************************************
		//FUNCTION:
		bool tryPop(int vTimeout, TOutputData& voOutput)
		{
			_ASSERTE(m_pOutputBuffer);
			std::unique_lock<std::mutex> Lock(m_OutputBufferReadableMutex);
			m_OutputBufferReadableSignal.wait_for(Lock, std::chrono::milliseconds(vTimeout), std::bind(&IPipeline::isOutputBufferReadable, this));
			if (!isOutputBufferReadable()) return false;

			bool r = tryPop(voOutput);
			_ASSERTE(r);
			return true;
		}

		//*****************************************************************************************
		//FUNCTION:
		void forcePushAllDataFromInput2OutputBuffer(int vTimeout = -1)
		{
			TInputData Input;
			TOutputData Output;

			_ASSERTE(m_pInputBuffer && m_pOutputBuffer);
			while (!m_pInputBuffer->empty())
			{
				bool r = __popFromInputBufferV(Input);
				_ASSERTE(r);
				Output = __convertDataV(Input);
				r = _push2OutputBuffer(Output);
				if (!r)
				{
					_waitUntilOutputBufferWritable(vTimeout);
					r = _push2OutputBuffer(Output);
					_ASSERTE(r);
				}
			}
		}

		//*****************************************************************************************
		//FUNCTION:
		void convertDataTask()
		{
			TInputData  Input;
			TOutputData Output;

			_waitUntilInputBufferReadable();
			bool r = __popFromInputBufferV(Input);
			_ASSERTE(r);
			Output = __convertDataV(Input);
			_waitUntilOutputBufferWritable();
			r = _push2OutputBuffer(Output);
			_ASSERTE(r);
		}

		//*****************************************************************************************
		//FUNCTION:
		void initInputBuffer()
		{
			_ASSERTE(!m_pInputBuffer);
			m_pInputBuffer = std::make_shared<TInputBuffer>();
		}

		//*****************************************************************************************
		//FUNCTION:
		void initOutputBuffer()
		{
			_ASSERTE(!m_pOutputBuffer);
			m_pOutputBuffer = std::make_shared<TOutputBuffer>();
		}

		//*****************************************************************************************
		//FUNCTION:
		void shareInputBufferWith(IPipeline<TInputData, TInputBuffer, TOutputData, TOutputBuffer>* vPipeline)
		{
			_ASSERTE(!m_pInputBuffer && vPipeline && vPipeline->m_pInputBuffer);
			m_pInputBuffer = vPipeline->m_pInputBuffer;
		}

		//*****************************************************************************************
		//FUNCTION:
		void shareOutputBufferWith(IPipeline<TInputData, TInputBuffer, TOutputData, TOutputBuffer>* vPipeline)
		{
			_ASSERTE(!m_pOutputBuffer && vPipeline && vPipeline->m_pOutputBuffer);
			m_pOutputBuffer = vPipeline->m_pOutputBuffer;
		}

		//*****************************************************************************************
		//NOTE: 注意这个函数中调用了虚函数，因此至少不要在基类IPipeline的析构函数中调用这个函数！！！
		void clear()
		{
			TInputData Input;
			while (!m_pInputBuffer->empty())
			{
				bool r = __popFromInputBufferV(Input);
				_ASSERTE(r);
				Input.destroy();
			}

			TOutputData Output;
			while (!m_pOutputBuffer->empty())
			{
				bool r = __popFromOutputBufferV(Output);
				_ASSERTE(r);
				Output.destroy();
			}
		}

		bool isInputBufferEmpty() const { _ASSERTE(m_pInputBuffer); return m_pInputBuffer->empty(); }
		bool isOutputBufferEmpty() const { _ASSERTE(m_pOutputBuffer); return m_pOutputBuffer->empty(); }
		bool isInputBufferReadable() const { _ASSERTE(m_pInputBuffer); return !isInputBufferEmpty(); }
		bool isOutputBufferReadable() const { _ASSERTE(m_pOutputBuffer); return !isOutputBufferEmpty(); }
		bool isOutputBufferWritable() const { return m_IsOutputBufferWritable; }

		void setAsynConvertDataHint() { m_IsAysnConvertData = true; }

		int getNumDataPushed2InputBuffer()  const { return m_PerfStatistic.NumDataPush2InputBuffer; }
		int getNumDataPushed2OutputBuffer() const { return m_PerfStatistic.NumDataPush2OutputBuffer; }
		int getNumDataPopFromOutputBuffer() const { return m_PerfStatistic.NumDataPopFromOutputBuffer; }

		virtual int getNumDataInOutputBufferV() const = 0;
		virtual int getNumDataInInputBufferV()  const = 0;

	protected:
		std::shared_ptr<TInputBuffer>  m_pInputBuffer;   //注意这里m_pInputBuffer和m_pOutputBuffer都采用了智能指针的形式，其原因是多个pipeline对象可能共享
		std::shared_ptr<TOutputBuffer> m_pOutputBuffer;  //一个m_pInputBuffer/m_pOutputBuffer（多个decoder的输出pipeline，可能共享一个m_pOutputBuffer），
															//如果不采用智能指针，将面临一个由哪个pipeline对象来负责删除m_pInputBuffer/m_pOutputBuffer的问题

		unsigned char m_InputBufferModel = 0;
		unsigned char m_OutputBufferModel = 0;

		void _setNumDataPush2OutputBuffer(int vNum) { m_PerfStatistic.NumDataPush2OutputBuffer = vNum; }

		void _waitUntilOutputBufferWritable(int vTimeout = -1)
		{
			std::unique_lock<std::mutex> Lock(m_OutputBufferWritableMutex);
			if (vTimeout > 0)
			{
				m_OutputBufferWritableSignal.wait_for(Lock, std::chrono::milliseconds(vTimeout), std::bind(&IPipeline::isOutputBufferWritable, this));
			}
			else
			{
				m_OutputBufferWritableSignal.wait(Lock, std::bind(&IPipeline::isOutputBufferWritable, this));
			}
		}

		void _waitUntilInputBufferReadable(int vTimeout = -1)
		{
			std::unique_lock<std::mutex> Lock(m_InputBufferReadableMutex);
			if (vTimeout > 0)
			{
				m_InputBufferReadableSignal.wait_for(Lock, std::chrono::milliseconds(vTimeout), std::bind(&IPipeline::isInputBufferReadable, this));
			}
			else
			{
				m_InputBufferReadableSignal.wait(Lock, std::bind(&IPipeline::isInputBufferReadable, this));
			}
		}

		bool _push2OutputBuffer(const TOutputData& vData)
		{
			bool r = __push2OutputBufferV(vData);
			if (r)
			{
				++m_PerfStatistic.NumDataPush2OutputBuffer;

				std::lock_guard<std::mutex> Lock(m_OutputBufferReadableMutex);
				m_OutputBufferReadableSignal.notify_one();
			}
			else
				m_IsOutputBufferWritable = false;
			return r;
		}

		bool _push2InputBuffer(const TInputData& vData)
		{
			bool r = __push2InputBufferV(vData);
			if (r)
			{
				++m_PerfStatistic.NumDataPush2InputBuffer;

				std::lock_guard<std::mutex> Lock(m_InputBufferReadableMutex);
				m_InputBufferReadableSignal.notify_one();
			}
			return r;
		}

	private:
		struct SPerformanceStatistic
		{
			int NumDataPush2InputBuffer = 0;
			int NumDataPush2OutputBuffer = 0;
			int NumDataPopFromOutputBuffer = 0;
		};

		bool m_IsAysnConvertData = false;
		bool m_IsOutputBufferWritable = true;  //注意这个变量实际等同于判断m_OutputBuffer是否还有空间可以容纳新的数据，即m_pOutputBuffer->size() < m_pOutputBuffer->capacity()。按道理来说，不应该使用m_IsOutputBufferWritable来保存m_OutputBuffer内部的状态。但注意
												//m_OutputBuffer实际包含模板，也就是说isOutputBufferWritable()应该实现为虚函数，针对不同的模板，可能有不同的实现。为了减少派生类去重载虚函数的工作量，这里我们用了额外的变量m_IsOutputBufferWritable，当往m_OutputBuffer
												//中压入数据失败时，将m_IsOutputBufferWritable设置为false
		std::mutex m_OutputBufferWritableMutex;
		std::mutex m_InputBufferReadableMutex;
		std::mutex m_OutputBufferReadableMutex;
		std::condition_variable m_OutputBufferReadableSignal;
		std::condition_variable m_OutputBufferWritableSignal;
		std::condition_variable m_InputBufferReadableSignal;
		SPerformanceStatistic   m_PerfStatistic;

		virtual TOutputData __convertDataV(const TInputData& vInput) = 0;
		virtual TInputData  __getFirstDataInInputBufferV() = 0;

		virtual void __popFirstDataInInputBufferV() = 0;
		virtual bool __popFromInputBufferV(TInputData& voData) = 0;
		virtual bool __popFromOutputBufferV(TOutputData& voData) = 0;
		virtual bool __push2InputBufferV(const TInputData& vData) = 0;
		virtual bool __push2OutputBufferV(const TOutputData& vData) = 0;
	};
}