/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/DirtyRegion.h"
#include "guilib/GUIControl.h"
#include "guilib/GUIListItemLayout.h"
#include "guilib/GUITexture.h"
#include "guilib/IGUIContainer.h"
#include "threads/CriticalSection.h"
#include "utils/Geometry.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class CDateTime;
class CFileItem;
class CFileItemList;
class CGUIListItem;
class CGUIListItemLayout;
class TiXmlElement;

namespace PVR
{
class CPVRChannel;
class CPVRChannelGroupMember;
class CPVRChannelNumber;

class CGUIEPGGridContainerModel;

class CGUIEPGGridContainer : public IGUIContainer
{
public:
  static constexpr unsigned int DEFAULT_MINUTES_PER_BLOCK{5};
  static constexpr unsigned int MINUTES_PER_RULER_UNIT{5};

  CGUIEPGGridContainer(int parentID,
                       int controlID,
                       float posX,
                       float posY,
                       float width,
                       float height,
                       ORIENTATION orientation,
                       int scrollTime,
                       int preloadItems,
                       int timeBlocks,
                       unsigned int minutesPerTimeBlock,
                       int rulerUnit,
                       const CTextureInfo& progressIndicatorTexture);
  CGUIEPGGridContainer(const CGUIEPGGridContainer& other);

  CGUIEPGGridContainer* Clone() const override { return new CGUIEPGGridContainer(*this); }

  /*!
   * @brief Check whether the control currently holds data.
   * @return true if the control has data, false otherwise.
   */
  bool HasData() const;

  void AllocResources() override;
  void FreeResources(bool immediately) override;

  bool OnAction(const CAction& action) override;
  void OnDown() override;
  void OnUp() override;
  void OnLeft() override;
  void OnRight() override;
  bool OnMouseOver(const CPoint& point) override;
  bool OnMessage(CGUIMessage& message) override;
  void SetFocus(bool focus) override;
  std::string GetDescription() const override;
  EVENT_RESULT OnMouseEvent(const CPoint& point, const KODI::MOUSE::CMouseEvent& event) override;

  void Process(unsigned int currentTime, CDirtyRegionList& dirtyregions) override;
  void Render() override;

  std::shared_ptr<CGUIListItem> GetListItem(int offset, unsigned int flag = 0) const override;
  std::string GetLabel(int info) const override;

  std::shared_ptr<CFileItem> GetSelectedGridItem(int offset = 0) const;
  std::shared_ptr<CPVRChannelGroupMember> GetSelectedChannelGroupMember() const;
  CDateTime GetSelectedDate() const;

  void LoadLayout(TiXmlElement* layout);
  void SetPageControl(int id);

  /*! \brief Set the offset of the first item in the container from the container's position
   Useful for lists/panels where the focused item may be larger than the non-focused items and thus
   normally cut off from the clipping window defined by the container's position + size.
   \param offset CPoint holding the offset in skin coordinates.
   */
  void SetRenderOffset(const CPoint& offset);

  void JumpToNow();
  void JumpToDate(const CDateTime& date);

  void GoToBegin();
  void GoToEnd();
  void GoToNow();
  void GoToDate(const CDateTime& date);

  void GoToFirstChannel();
  void GoToLastChannel();

  void GoToTop();
  void GoToBottom();
  void GoToMostLeft();
  void GoToMostRight();

  void SetTimelineItems(const CFileItemList& items,
                        const CDateTime& gridStart,
                        const CDateTime& gridEnd);

  std::unique_ptr<CFileItemList> GetCurrentTimeLineItems() const;

  /*!
   * @brief Set the control's selection to the given channel and set the control's view port to show the channel.
   * @param channel the channel.
   * @return true if the selection was set to the given channel, false otherwise.
   */
  bool SetChannel(const std::shared_ptr<CPVRChannel>& channel);

  /*!
   * @brief Set the control's selection to the given channel and set the control's view port to show the channel.
   * @param channel the channel's path.
   * @return true if the selection was set to the given channel, false otherwise.
   */
  bool SetChannel(std::string_view channel);

  /*!
   * @brief Set the control's selection to the given channel and set the control's view port to show the channel.
   * @param channelNumber the channel's number.
   * @return true if the selection was set to the given channel, false otherwise.
   */
  bool SetChannel(const CPVRChannelNumber& channelNumber);

  void AssignDepth() override;

  void AssignItemDepth(CGUIListItem* item, bool focused) const;

private:
  bool OnClick(int actionID) const;
  bool SelectItemFromPoint(const CPoint& point, bool justGrid = true);

  void SetChannel(int channel);

  void SetBlock(int block, bool bUpdateBlockTravelAxis = true);
  void UpdateBlock(bool bUpdateBlockTravelAxis = true);

  void ChannelScroll(int amount);
  void ProgrammesScroll(int amount);
  void ValidateOffset();
  void UpdateLayout();

  void SetItem(const std::pair<std::shared_ptr<CFileItem>, int>& itemInfo);
  bool SetItem(const std::shared_ptr<CFileItem>& item, int channelIndex, int blockIndex);
  std::shared_ptr<CFileItem> GetItem() const;
  std::pair<std::shared_ptr<CFileItem>, int> GetNextItem() const;
  std::pair<std::shared_ptr<CFileItem>, int> GetPrevItem() const;
  void UpdateItem();

  void MoveToRow(int row);

  CGUIListItemLayout* GetFocusedLayout() const;

  void ScrollToBlockOffset(int offset);
  void ScrollToChannelOffset(int offset);
  void GoToBlock(int blockIndex);
  void GoToChannel(int channelIndex);
  void UpdateScrollOffset(unsigned int currentTime);
  void ProcessItem(float posX,
                   float posY,
                   const std::shared_ptr<CFileItem>& item,
                   std::shared_ptr<CFileItem>& lastitem,
                   bool focused,
                   CGUIListItemLayout* normallayout,
                   CGUIListItemLayout* focusedlayout,
                   unsigned int currentTime,
                   CDirtyRegionList& dirtyregions,
                   float resize = -1.0f);
  void RenderItem(float posX, float posY, CGUIListItem* item, bool focused) const;
  void GetCurrentLayouts();

  void ProcessChannels(unsigned int currentTime, CDirtyRegionList& dirtyregions);
  void ProcessRuler(unsigned int currentTime, CDirtyRegionList& dirtyregions);
  void ProcessRulerDate(unsigned int currentTime, CDirtyRegionList& dirtyregions);
  void ProcessProgrammeGrid(unsigned int currentTime, CDirtyRegionList& dirtyregions);
  void ProcessProgressIndicator(unsigned int currentTime);
  void RenderChannels();
  void RenderRulerDate();
  void RenderRuler();
  void RenderProgrammeGrid();
  void RenderProgressIndicator();

  void GetChannelCacheOffsets(int& cacheBefore, int& cacheAfter) const;
  void GetProgrammeCacheOffsets(int& cacheBefore, int& cacheAfter) const;

  bool OnMouseClick(int dwButton, const CPoint& point);
  bool OnMouseDoubleClick(int dwButton, const CPoint& point);
  bool OnMouseWheel(char wheel);

  void HandleChannels(bool bRender,
                      unsigned int currentTime,
                      CDirtyRegionList& dirtyregions,
                      bool bAssignDepth = false);
  void HandleRuler(bool bRender,
                   unsigned int currentTime,
                   CDirtyRegionList& dirtyregions,
                   bool bAssignDepth = false);
  void HandleRulerDate(bool bRender,
                       unsigned int currentTime,
                       CDirtyRegionList& dirtyregions,
                       bool bAssignDepth = false);
  void HandleProgrammeGrid(bool bRender,
                           unsigned int currentTime,
                           CDirtyRegionList& dirtyregions,
                           bool bAssignDepth = false);

  float GetCurrentTimePositionOnPage() const;
  float GetProgressIndicatorWidth() const;
  float GetProgressIndicatorHeight() const;

  void UpdateItems();

  float GetChannelScrollOffsetPos() const;
  float GetProgrammeScrollOffsetPos() const;
  int GetChannelScrollOffset(const CGUIListItemLayout& layout) const;
  int GetProgrammeScrollOffset() const;

  int GetBlockScrollOffset() const;

  CPoint m_renderOffset; ///< \brief render offset of the first item in the list \sa SetRenderOffset

  ORIENTATION m_orientation{VERTICAL};

  std::vector<CGUIListItemLayout> m_channelLayouts;
  std::vector<CGUIListItemLayout> m_focusedChannelLayouts;
  std::vector<CGUIListItemLayout> m_focusedProgrammeLayouts;
  std::vector<CGUIListItemLayout> m_programmeLayouts;
  std::vector<CGUIListItemLayout> m_rulerLayouts;
  std::vector<CGUIListItemLayout> m_rulerDateLayouts;

  CGUIListItemLayout* m_channelLayout{nullptr};
  CGUIListItemLayout* m_focusedChannelLayout{nullptr};
  CGUIListItemLayout* m_programmeLayout{nullptr};
  CGUIListItemLayout* m_focusedProgrammeLayout{nullptr};
  CGUIListItemLayout* m_rulerLayout{nullptr};
  CGUIListItemLayout* m_rulerDateLayout{nullptr};

  int m_pageControl{0};

  const int m_blocksPerRulerItem{0}; //! number of blocks that make up one element of the ruler
  const int m_blocksPerPage{0};
  const unsigned int m_minutesPerBlock{DEFAULT_MINUTES_PER_BLOCK};

  int m_channelsPerPage{0};
  int m_programmesPerPage{0};
  int m_channelCursor{0};
  int m_channelOffset{0};
  int m_blockCursor{0};
  int m_blockOffset{0};
  int m_blockTravelAxis{0};
  int m_cacheChannelItems{0};
  int m_cacheProgrammeItems{0};
  int m_cacheRulerItems{0};

  float m_rulerDateHeight{0.0f}; //! height of ruler date item
  float m_rulerDateWidth{0.0f}; //! width of ruler date item
  float m_rulerPosX{0.0f}; //! X position of first ruler item
  float m_rulerPosY{0.0f}; //! Y position of first ruler item
  float m_rulerHeight{0.0f}; //! height of the scrolling timeline above the ruler items
  float m_rulerWidth{0.0f}; //! width of each element of the ruler
  float m_channelPosX{0.0f}; //! X position of first channel row
  float m_channelPosY{0.0f}; //! Y position of first channel row
  float m_channelHeight{0.0f}; //! height of the channel item
  float m_channelWidth{0.0f}; //! width of the channel item
  float m_gridPosX{0.0f}; //! X position of first grid item
  float m_gridPosY{0.0f}; //! Y position of first grid item
  float m_gridWidth{0.0f}; //! width of the epg grid control
  float m_gridHeight{0.0f}; //! height of the epg grid control
  float m_blockSize{0.0f}; //! a block's width in pixels
  float m_analogScrollCount{0.0f};

  std::unique_ptr<CGUITexture> m_guiProgressIndicatorTexture;
  uint32_t m_guiProgressIndicatorTextureDepth{0};

  std::shared_ptr<CFileItem> m_lastItem;
  std::shared_ptr<CFileItem> m_lastChannel;

  bool m_bEnableProgrammeScrolling{true};
  bool m_bEnableChannelScrolling{true};

  int m_scrollTime{0};

  unsigned int m_programmeScrollLastTime{0};
  float m_programmeScrollSpeed{0.0f};
  float m_programmeScrollOffset{0.0f};

  unsigned int m_channelScrollLastTime{0};
  float m_channelScrollSpeed{0.0f};
  float m_channelScrollOffset{0.0f};

  mutable CCriticalSection m_critSection;
  std::unique_ptr<CGUIEPGGridContainerModel> m_gridModel;
  std::unique_ptr<CGUIEPGGridContainerModel> m_updatedGridModel;

  int m_itemStartBlock{0};
};
} // namespace PVR
