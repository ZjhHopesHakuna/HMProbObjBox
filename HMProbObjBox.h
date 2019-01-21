#pragma once
#include <limits>
#include <vector>
#include <iostream>

template <typename T1>
class CHMProbObjBox
{
public:
	CHMProbObjBox() : m_unCurrentProbObjCount(0)
	{
		std::vector<std::pair<T1, unsigned int>>().swap(m_vecProbObjPool);
	}
	~CHMProbObjBox() {};

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Draw a probability object from this box.
	// t1ProbObj:	If this call succeed, the drawn probability object will be put into
	//				t1ProbObj.
	// nRand:		It decides which probability object will be drawn. It should be a positive
	//				random int value(recommended), or -1.
	// Return:		Return true if succeed, false if failed.
	bool Draw(T1& t1ProbObj, const int nRand = -1)
	{
		if (0 == m_unCurrentProbObjCount) return false;
		if (nRand < 0 && -1 != nRand) return false;

		unsigned int unRand = (0 > nRand) ? rand() : nRand;
		unsigned int unKeyNum = unRand % m_unCurrentProbObjCount;

		return FindProbObjByRandKey(t1ProbObj, unKeyNum);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Modify probability objects of this box, so that we can get a proper
	//				probability objects box we wanted.
	// t1ProbObj:	A pointer to the storage of object that you want to modify.
	// pCount:		A pointer to the storage of every object's new count.
	// unLen:		The length of storage.
	// Return:		None.
	void Modify(const T1* const t1ProbObj, const unsigned int* const pCount, const unsigned int unLen = 1)
	{
		if (NULL == t1ProbObj || NULL == pCount || 0 == unLen) return;

		for (unsigned int i = 0; i < unLen; i++)
		{
			ModifyProbObjPool(t1ProbObj[i], pCount[i]);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Modify probability objects of this box, so that we can get a proper
	//				probability objects box we needed.
	// t2ProbObj:	A user-defined container which contains the probability objects and
	//				their counts we want.
	// Return:		None.
	template <typename T2>
	void Modify(const T2& t2ProbObj)
	{
		for (auto it = t2ProbObj.cbegin(); it != t2ProbObj.cend(); it++)
		{
			ModifyProbObjPool(it->first, it->second);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Clear this box to make it empty.
	// Return:		None.
	void Clear()
	{
		m_unCurrentProbObjCount = 0;
		std::vector<std::pair<T1, unsigned int>>().swap(m_vecProbObjPool);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get particular or total probability objects counts, depend on pProbObj.
	// pProbObj:	A pointer of the specific probability object, if it == NULL, get the
	//				total count.
	// Return:		The count.
	unsigned int GetCount(const T1* const pProbObj = NULL) const
	{
		if (NULL == pProbObj) return m_unCurrentProbObjCount;

		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			if (*pProbObj == it->first) return it->second;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get the probability objects pool.
	// Return:		The probability objects pool.
	const auto& GetPool() const{ return m_vecProbObjPool; }

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Dump the details of this box.
	// Return:		None.
	void Dump() const
	{
		std::cout << "Current total probability object count " << m_unCurrentProbObjCount << "." << std::endl;
		std::cout << "Probability object box capacity " << m_scunProbObjBoxCapacity << std::endl;

		int nIndex = 0;
		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			nIndex++;
			std::cout << "Probability object index " << nIndex << ", count " << it->second << "." << std::endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get the version of CHMProbObjBox.
	// Return:		A unsigned int stands for the version.
	unsigned int Version() const { return m_scunCHMProbObjBoxVersion; }

private:
	unsigned int m_unCurrentProbObjCount;
	std::vector<std::pair<T1, unsigned int>> m_vecProbObjPool;
	static const unsigned int m_scunProbObjBoxCapacity = UINT_MAX;
	static const unsigned int m_scunCHMProbObjBoxVersion = 2;

	void ModifyProbObjPool(const T1& t1ProbObj, const unsigned int unCount)
	{
		for (auto it = m_vecProbObjPool.begin(); it != m_vecProbObjPool.end(); it++)
		{
			if (t1ProbObj == it->first)
			{
				if (0 == unCount)
				{
					m_unCurrentProbObjCount = m_unCurrentProbObjCount - it->second + unCount;
					m_vecProbObjPool.erase(it);
				}
				else if (m_scunProbObjBoxCapacity - unCount >= m_unCurrentProbObjCount - it->second)
				{
					it->second = unCount;
					m_unCurrentProbObjCount = m_unCurrentProbObjCount - it->second + unCount;
				}
				return;
			}
		}

		if (0 != unCount && m_scunProbObjBoxCapacity - unCount > m_unCurrentProbObjCount)
		{
			try
			{
				m_vecProbObjPool.push_back(std::make_pair(t1ProbObj, unCount));
			}
			catch (const std::exception& e)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), exception: " << e.what() << std::endl;
			}
			catch (...)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), unknow exception" << std::endl;
			}
			m_unCurrentProbObjCount += unCount;
		}
	}

	bool FindProbObjByRandKey(T1& t1ProbObj, const unsigned int unRandKey) const
	{
		unsigned int unBotton = 0, unTop = 0;
		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			unBotton = unTop;
			unTop += it->second;
			if (unRandKey >= unBotton && unRandKey < unTop)
			{
				t1ProbObj = it->first;
				return true;
			}
		}
		return false;
	}
};
