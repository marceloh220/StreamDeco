/**
 * Copyright © 2024 Marcelo H Moraes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _STREAMDECO_BUTTONS_HPP_
#define _STREAMDECO_BUTTONS_HPP_

#include "lvgl.hpp"
#include "marcelino.hpp"

namespace streamDeco
{
 
  /**
   * @class   MainButton
   * @brief    Create a streamDecoButtons class with some predefined things inside
   */
  class MainButton : public lvgl::Object
  {
  public:

    /**
     * @brief   Start object with some configuration predefined
     * @param   text Text to be showed on streamDecoButtons object
     * @param   icon1 First icon to be showed on streamDecoButtons object
     * @param   icon2 Second icon to be showed on streamDecoButtons object
     * @note    If no icons are passsed, or passed as NULL, only text will be showed on streamDecoButtons.
     *          If two icons are passed they can be switched using Button:iconSwap() method
     */
    MainButton(const char *text, lvgl::icon_t icon1 = NULL, lvgl::icon_t icon2 = NULL)
    : text_scr(text), icon1_scr(icon1), icon2_scr(icon2) {
    }

    /**
    * @brief  Create a new streamDecoButtons
    * @param  pos streamDecoButtons position
    * @note   The new streamDecoButtons will be created into main screen
    * @note   The pos is based on fixed coordnates matrix
    */
    void create(uint8_t pos, lvgl::palette::palette_t color);

    /**
    * @brief  Create a new streamDecoButtons
    * @param  parent object parent of the new streamDecoButtons
    * @param  pos streamDecoButtons position
    * @note   The new streamDecoButtons will be created into main screen
    * @note   The pos is based on fixed coordnates matrix
    */
    void create(Object &parent, uint8_t pos, lvgl::palette::palette_t color);

    /**
     * @brief   Change streamDecoButtons color
     * @param   color Palette of streamDecoButtons color
     */
    void buttonColor(lvgl::palette::palette_t color);
    
    /**
     * @brief   Change streamDecoButtons pinned color
     * @param   color Palette of streamDecoButtons pinned color
     */
    void buttonPinnedColor(lvgl::palette::palette_t color);
    
    /**
     * @brief   Change icon color
     * @param   color Palette of icon color
     */
    void iconColor(lvgl::palette::palette_t color);
    
    /**
     * @brief   Change icon color
     * @param   color Color of icon
     */
    void iconColor(lv_color_t color);
    
    /**
     * @brief   Change icon pinned color
     * @param   color Color of icon pinned
     */
    void iconPinnedColor(lvgl::palette::palette_t color);
    
    /**
     * @brief   Change icon pinned color
     * @param   color Color of icon pinned
     */
    void iconPinnedColor(lv_color_t color);
    
    /**
     * @sa      lvgl::event::code_t
     * @brief   Register a function callback on streamDecoButtons
     * @param   callback Function callbatk that will be called when streamDecoButtons capture an action
     * @param   code Button action the will trigger the callback function
     * @note    The callback function need be void return type and receive an lvgl::event::event_t parameter
     */
    void callback(lvgl::event::callback_t callback, lvgl::event::code_t code, int user_data);
    
    /**
     * @brief   If using two icons change between then
     */
    void iconSwap();
    
    /**
     * @brief   Pin the streamDecoButtons
     */
    void pin();
    
    /**
     * @brief   Unpin the streamDecoButtons
     */
    void unpin();
    
    /**
     * @brief   Verify if streamDecoButtons is pinned
     * @return  Return streamDecoButtons pin state
     *            0 - Button is not pinned
     *            1 - Button is pinned
     */
    bool pinned();
    
    /**
     * @brief Change streamDecoButtons position
     * @note   The pos is based on fixed coordnates matrix
     * @note   The main streamDecoButtons have an position pool
     */
    virtual void position(uint8_t pos);
  
  protected:
  
    /**
     * @brief   Internal init streamDecoButtons members
     * @note    This is a protected member
     */
    void init(lvgl::palette::palette_t color);
  
    virtual bool assertPosition(uint8_t pos)
    {
      return (pos >= 15) ? false : true;
    }
  
    /**
     * @var     label
     * @brief   Button label
     * @details Used if streamDecoButtons have no icon
     * @note    This is a protected member
     */
    lvgl::Label label;

    /**
     * @var     icon
     * @brief   Button icon
     * @note    This is a protected member
     */
    lvgl::Image icon;

    lvgl::Style style_button;
    lvgl::Style style_buttonPressed;
    lvgl::Style style_buttonPinned;
    lvgl::Style style_icon;
    lvgl::Style style_iconPinned;
    
    /**
     * @var     text_scr
     * @brief   Label text source
     * @note    This is a protected member
     */
    const char *text_scr;

    /**
     * @var     icon1_scr
     * @brief   Icon 1 source of icon
     * @note    This is a protected member
     */
    lvgl::icon_t icon1_scr;

    /**
     * @var     icon2_scr
     * @brief   Icon 1 source of icon
     * @note    This is a protected member
     */
    lvgl::icon_t icon2_scr;

    /**
     * @struct  state_pack
     * @brief   Keep the icon states
     * @note    This is a protected member
     */
    struct state_pack{
      uint8_t icon_now:1 = true;
      uint8_t pinnedState:1 = false;
    } state;

  }; // class MainButton

  /**
   * @class CanvasButton
   * 
   */
  class CanvasButton : public MainButton
  {
  public:
    CanvasButton(const char *text, lvgl::icon_t icon1 = NULL, lvgl::icon_t icon2 = NULL)
        : MainButton(text, icon1, icon2) {}
    void position(uint8_t pos);
  private:
    bool assertPosition(uint8_t pos)
    {
      return (pos >= 9) ? false : true;
    }
  }; // class CanvasButton

  class ConfigButton : public MainButton
  {
  public:
    ConfigButton(const char *text, lvgl::icon_t icon1 = NULL, lvgl::icon_t icon2 = NULL)
        : MainButton(text, icon1, icon2) {}
    void position(uint8_t pos);
  private:
    bool assertPosition(uint8_t pos)
    {
      return (pos >= 9) ? false : true;
    }
  }; // class ConfigButton

} // namespace streamDeco

#endif
