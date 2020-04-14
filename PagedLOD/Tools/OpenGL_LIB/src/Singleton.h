#pragma once

template<class T>
class CSingleton {
public:
	static T& getInstance() {
		static T instance;
		return instance;
	}
protected:
	CSingleton() = default;
	virtual ~CSingleton() = default;
};