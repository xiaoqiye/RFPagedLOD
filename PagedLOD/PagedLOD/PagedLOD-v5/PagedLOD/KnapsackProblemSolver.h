#pragma once

#include "Common.h"

namespace hivePagedLOD
{
	struct SKPSolution
	{
		unsigned int X;
		unsigned int Y;
		uintmax_t TriangleCount;
		SKPSolution() = default;
		SKPSolution(unsigned int vX, unsigned int vY, uintmax_t vTriangleCount) : X(vX), Y(vY), TriangleCount(vTriangleCount) {}
	};
	
	class CKnapsackProblemSolver
	{
	public:
		CKnapsackProblemSolver() = default;
		static std::vector<std::vector<SKnapsackItem>> prepareKnapsackItem(const SPreferredResult& vPreferredResult, const std::vector<std::vector<SKnapsackItem>>& vAllTemplateItemSet);
		static void solveMultipleChoiceKnapsackProblem(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit, std::vector<unsigned int>& voNotLoadTileNumSet, std::vector<SKnapsackItem>& voResultItemSet);

	private:
		static std::vector<SKnapsackItem> __generateOneTileKnapsackItemSet(const std::vector<std::shared_ptr<CTileNode>>& vOneTilePreferredTileNodeSet, const std::vector<SKnapsackItem>& vOneTileTemplateSet, unsigned int vMaxDeep);
		static std::vector<std::vector<SKPSolution>> __fillDPTable(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit);
		static void __initDPTable(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit, std::vector<std::vector<uintmax_t>>& voDPTable, std::vector<std::vector<SKPSolution>>& voSolution);
		static void __generateKPResultItemSet(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, const std::vector<std::vector<SKPSolution>>& vSolution, std::vector<unsigned int>& voNotLoadTileNumSet, std::vector<SKnapsackItem>& voResultSet);
	};
}
