#pragma once
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include "BoundingSphere.h"

//PagedLOD Tile.h
namespace hivePagedLOD
{
	struct STileNodeStatus
	{
		STileNodeStatus() = default;
		STileNodeStatus(bool vShow, bool vLoad, bool vVisible, bool vMatchLOD) : NeedShow(vShow), NeedLoad(vLoad), IsVisible(vVisible), MatchLOD(vMatchLOD) {}
		bool NeedShow = false;
		bool NeedLoad = false;
		bool IsVisible = false;
		bool MatchLOD = false;
		void resetStatus() { NeedShow = false; NeedLoad = false; IsVisible = false; MatchLOD = false; }
		////NeedShow  0 1 1
		////NeedLoad  0 1 0
		////
		////IsVisible 0 1 1
		////MatchLOD  0 1 0
	};
	
	class CScene;
	class CTileNode
	{
	public:

		CTileNode() :
			CTileNode(std::weak_ptr<CTileNode>()) {}

		CTileNode(std::weak_ptr<CTileNode> vParent) :
			m_UID(0),
			m_GeometryFileName(""),
			m_GeometryFileSize(0),
			m_TextureFileName(""),
			m_TextureFileSize(0),
			m_TriangleCount(0),
			m_NodeDeep(0),
			m_LODLevel(0),
			m_pParent(std::move(vParent)) {}
		
		~CTileNode()
		{
			clearChildren();
			clearBrother();
		}

		unsigned int getUID() const { return m_UID; }
		const CBoundingSphere& getBoundingSphere() const { return m_BoundingSphere; }
		const std::string& getGeometryFileName() const { return m_GeometryFileName; }
		const std::string& getTextureFileName() const { return m_TextureFileName; }
		uintmax_t getGeometryFileSize() const { return m_GeometryFileSize; }
		uintmax_t getTextureFileSize() const { return m_TextureFileSize; }
		uintmax_t getTriangleCount() const { return m_TriangleCount; }
		std::weak_ptr<CTileNode> getParent() const { return m_pParent; }
		unsigned int getNodeDeep() const { return m_NodeDeep; }
		int getLODLevel() const { return m_LODLevel; }
		const std::vector<unsigned int>& getAncestorUIDSet() const { return m_AncestorUIDSet; }
		const std::vector<unsigned int>& getBrotherUIDSet() const { return m_BrotherUIDSet; }

		std::size_t getNumChildren() const { return m_Children.size(); }
		void addChild(const std::shared_ptr<CTileNode>& vChild) { m_Children.push_back(vChild); }
		void clearChildren() { m_Children.clear(); }

		void addBrother(const std::shared_ptr<CTileNode>& vBrother) { m_Brother.push_back(vBrother); }
		const std::vector<std::weak_ptr<CTileNode>>& getBrotherSet() const { return m_Brother; }
		void clearBrother() { m_Brother.clear(); }
		
		CTileNode& getChildAt(std::vector<std::shared_ptr<CTileNode>>::size_type vIndex) const { return *m_Children.at(vIndex); }
		std::shared_ptr<CTileNode>& fetchChildPointerAt(std::vector<std::shared_ptr<CTileNode>>::size_type vIndex) { _ASSERTE(m_Children.size() > vIndex); return m_Children[vIndex]; }
		const std::shared_ptr<CTileNode>& getChildPointerAt(std::vector<std::shared_ptr<CTileNode>>::size_type vIndex) const { _ASSERTE(m_Children.size() > vIndex); return m_Children[vIndex]; }
		std::vector<std::shared_ptr<CTileNode>>& fetchAllChildren() { return m_Children; }
		const std::vector<std::shared_ptr<CTileNode>>& getAllChildren() const { return m_Children; }

		const STileNodeStatus& getStatus() const { return m_Status; }
		void resetStatus() { m_Status.resetStatus(); }
		
		void setUID(unsigned int vUID) { m_UID = vUID; }
		void setBoundingSphere(const CBoundingSphere& vBoundingSphere) { m_BoundingSphere = vBoundingSphere; }
		void setGeometryFileName(const std::string& vFileName) { m_GeometryFileName = vFileName; }
		void setGeometryFileSize(std::uintmax_t vFileSize) { m_GeometryFileSize = vFileSize; }
		void setTextureFileName(const std::string& vFileName) { m_TextureFileName = vFileName; }
		void setTextureFileSize(std::uintmax_t vFileSize) { m_TextureFileSize = vFileSize; }
		void setTriangleCount(std::uintmax_t vTriangleCount) { m_TriangleCount = vTriangleCount; }
		void setNodeDeep(unsigned int vDeep) { m_NodeDeep = vDeep; }
		void setLODLevel(int vLevel) { m_LODLevel = vLevel; }
		void setParent(const std::weak_ptr<CTileNode>& vParent) { m_pParent = vParent; }
		void setAncestorUIDSet(const std::vector<unsigned int>& vSet) { m_AncestorUIDSet = vSet; }
		void setBrotherUIDSet(const std::vector<unsigned int>& vSet) { m_BrotherUIDSet = vSet; }

		void setNeedShow(bool vFlag) { m_Status.NeedShow = vFlag; }
		void setNeedLoad(bool vFlag) { m_Status.NeedLoad = vFlag; }
		void setIsVisible(bool vFlag) { m_Status.IsVisible = vFlag; }
		void setMatchLOD(bool vFlag) { m_Status.MatchLOD = vFlag; }

#ifdef _UNIT_TEST
		std::string m_TileNodeName;
		void setTileNodeName(const std::string& vTileNodeName) { _ASSERT(!vTileNodeName.empty()); m_TileNodeName = vTileNodeName; }
		const std::string& getTileNodeName() const { return m_TileNodeName; }
#endif

	private:
		unsigned int m_UID;
		CBoundingSphere m_BoundingSphere;
		std::string m_GeometryFileName;
		std::uintmax_t m_GeometryFileSize;
		std::string m_TextureFileName;
		std::uintmax_t m_TextureFileSize;
		std::uintmax_t m_TriangleCount;
		unsigned int m_NodeDeep;
		int m_LODLevel;
		std::vector<std::shared_ptr<CTileNode>> m_Children;
		std::weak_ptr<CTileNode> m_pParent;
		std::vector<unsigned int> m_AncestorUIDSet;
		std::vector<unsigned int> m_BrotherUIDSet;

		std::vector<std::weak_ptr<CTileNode>> m_Brother;
		
		STileNodeStatus m_Status;
		
		friend class hivePagedLOD::CScene;
	};
}