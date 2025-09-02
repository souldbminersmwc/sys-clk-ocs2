/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "freq_choice_gui.h"

#include "../format.h"
#include "fatal_gui.h"

class ColoredListItem : public tsl::elm::ListItem
{
    tsl::Color color;

public:
    ColoredListItem(const std::string &text, const std::string &value, bool isMini, tsl::Color c)
        : tsl::elm::ListItem(text, value, isMini), color(c) {}

    void draw(tsl::gfx::Renderer *renderer) override
    {
        tsl::elm::ListItem::draw(renderer);

        u16 tx = this->getX() + 19;
        u16 ty = this->getY() + (this->getHeight() / 2) + 7; // vertically center-ish
        u8 fontSize = 23;

        renderer->drawString(this->getText().c_str(), false, tx, ty, fontSize, this->color);

        if (!this->getValue().empty())
        {
            renderer->drawString(this->getValue().c_str(), true, this->getX() + this->getWidth() - 20, ty, fontSize, tsl::style::color::ColorText);
        }
    }
};

FreqChoiceGui::FreqChoiceGui(std::uint32_t selectedHz, std::uint32_t *hzList, std::uint32_t hzCount, SysClkModule module, FreqChoiceListener listener)
{
    this->selectedHz = selectedHz;
    this->hzList = hzList;
    this->hzCount = hzCount;
    this->module = module; // Add this
    this->listener = listener;
}

tsl::elm::ListItem *FreqChoiceGui::createFreqListItem(std::uint32_t hz, bool selected, int safety)
{
    tsl::elm::ListItem *listItem;

    switch (safety)
    {
    case 0:
        listItem = new tsl::elm::ListItem(formatListFreqHz(hz), "", true);
        break;
    case 1:
        listItem = new ColoredListItem(formatListFreqHz(hz), "", true, tsl::Color(255, 165, 0, 255));
        break;
    case 2:
        listItem = new ColoredListItem(formatListFreqHz(hz), "", true, tsl::Color(255, 0, 0, 255));
        break;
    }

    listItem->setValue(selected ? "\uE14B" : "");

    listItem->setClickListener([this, hz](u64 keys)
                               {
        if ((keys & HidNpadButton_A) == HidNpadButton_A && this->listener) {
            if (this->listener(hz)) {
                tsl::goBack();
            }
            return true;
        }
        return false; });

    return listItem;
}

void FreqChoiceGui::listUI()
{
    // Add CategoryHeader based on module
    std::string moduleName = sysclkFormatModule(this->module, true);
    this->listElement->addItem(new tsl::elm::CategoryHeader(moduleName));

    this->listElement->addItem(this->createFreqListItem(0, this->selectedHz == 0, false));
    std::uint32_t hz;
    for (std::uint32_t i = 0; i < this->hzCount; i++)
    {
        hz = this->hzList[i];
        // Skip 204 MHz exactly
        if (moduleName == "Memory" && hz == 204000000)
        {
            continue;
        }
        uint32_t unsafe_cpu;
        uint32_t unsafe_gpu;
        uint32_t danger_cpu;
        uint32_t danger_gpu;
        if (IsMariko())
        {
            unsafe_cpu = 1964;
            unsafe_gpu = 1153;
            danger_cpu = 2398;
            danger_gpu = 1306;
        }
        else
        {
            unsafe_cpu = 1786;
            unsafe_gpu = 922;
            danger_cpu = 2092;
            danger_gpu = 999;
        }
            if ((moduleName == "CPU" && (hz / 1000000) >= danger_cpu) || (moduleName == "GPU" && (hz / 1000000) >= danger_gpu))
            {
                this->listElement->addItem(this->createFreqListItem(hz, (hz / 1000000) == (this->selectedHz / 1000000), 2));
                return;
            }
            else if ((moduleName == "CPU" && (hz / 1000000) >= unsafe_cpu) || (moduleName == "GPU" && (hz / 1000000) >= unsafe_gpu))
            {
                this->listElement->addItem(this->createFreqListItem(hz, (hz / 1000000) == (this->selectedHz / 1000000), 1));
                return;
            }
            else
            {
                this->listElement->addItem(this->createFreqListItem(hz, (hz / 1000000) == (this->selectedHz / 1000000), 0));
            }
    }
    this->listElement->jumpToItem("", "");
}