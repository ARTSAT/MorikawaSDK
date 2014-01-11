/*
**      ARTSAT MorikawaSDK
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      All rights reserved.
**
**      Version     Arduino
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Arduino IDE.
**      Arduino 1.0.5
**
**      TSTTrinity.h
**
**      ------------------------------------------------------------------------
**
**      GNU GENERAL PUBLIC LICENSE (GPLv3)
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License,
**      or (at your option) any later version.
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**      See the GNU General Public License for more details.
**      You should have received a copy of the GNU General Public License
**      along with this program. If not, see <http://www.gnu.org/licenses/>.
**
**      このプログラムはフリーソフトウェアです。あなたはこれをフリーソフトウェア財団によって発行された
**      GNU 一般公衆利用許諾書（バージョン 3 か、それ以降のバージョンのうちどれか）が定める条件の下で
**      再頒布または改変することができます。このプログラムは有用であることを願って頒布されますが、
**      *全くの無保証* です。商業可能性の保証や特定目的への適合性は、言外に示されたものも含め全く存在しません。
**      詳しくは GNU 一般公衆利用許諾書をご覧ください。
**      あなたはこのプログラムと共に GNU 一般公衆利用許諾書のコピーを一部受け取っているはずです。
**      もし受け取っていなければ <http://www.gnu.org/licenses/> をご覧ください。
*/

#ifndef __TST_TRINITY_H
#define __TST_TRINITY_H

#include "TSTType.h"

namespace tst {

template <typename T>
class TSTTrinity {
    private:
        mutable T volatile          _value[3];
    
    public:
        explicit                    TSTTrinity                  (void);
                                    TSTTrinity                  (TSTTrinity const& param);
                                    TSTTrinity                  (T param);
                                    ~TSTTrinity                 (void);
                TSTTrinity&         operator=                   (TSTTrinity const& param);
                TSTTrinity&         operator=                   (T param);
                TSTTrinity&         operator+=                  (T param);
                TSTTrinity&         operator-=                  (T param);
                TSTTrinity&         operator*=                  (T param);
                TSTTrinity&         operator/=                  (T param);
                TSTTrinity&         operator%=                  (T param);
                TSTTrinity&         operator&=                  (T param);
                TSTTrinity&         operator|=                  (T param);
                TSTTrinity&         operator^=                  (T param);
                TSTTrinity&         operator<<=                 (T param);
                TSTTrinity&         operator>>=                 (T param);
                TSTTrinity&         operator++                  (void);
                TSTTrinity          operator++                  (int);
                TSTTrinity&         operator--                  (void);
                TSTTrinity          operator--                  (int);
                                    operator T                  (void) const;
    private:
                void                set                         (T param);
                T                   get                         (void) const;
};
template <typename T>
class TSTTrinity<T*> {
    private:
        mutable T* volatile         _value[3];
    
    public:
        explicit                    TSTTrinity                  (void);
                                    TSTTrinity                  (TSTTrinity const& param);
                                    TSTTrinity                  (T* param);
                                    ~TSTTrinity                 (void);
                TSTTrinity&         operator=                   (TSTTrinity const& param);
                TSTTrinity&         operator=                   (T* param);
                TSTTrinity&         operator+=                  (int param);
                TSTTrinity&         operator-=                  (int param);
                TSTTrinity&         operator++                  (void);
                TSTTrinity          operator++                  (int);
                TSTTrinity&         operator--                  (void);
                TSTTrinity          operator--                  (int);
                T*                  operator->                  (void) const;
                                    operator T*                 (void) const;
    private:
                void                set                         (T* param);
                T*                  get                         (void) const;
};

template <typename T>
/*public */inline TSTTrinity<T>::TSTTrinity(void)
{
}

template <typename T>
/*public */inline TSTTrinity<T>::TSTTrinity(TSTTrinity const& param)
{
    set(param.get());
}

template <typename T>
/*public */inline TSTTrinity<T>::TSTTrinity(T param)
{
    set(param);
}

template <typename T>
/*public */inline TSTTrinity<T>::~TSTTrinity(void)
{
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator=(TSTTrinity const& param)
{
    set(param.get());
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator=(T param)
{
    set(param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator+=(T param)
{
    set(get() + param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator-=(T param)
{
    set(get() - param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator*=(T param)
{
    set(get() * param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator/=(T param)
{
    set(get() / param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator%=(T param)
{
    set(get() % param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator&=(T param)
{
    set(get() & param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator|=(T param)
{
    set(get() | param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator^=(T param)
{
    set(get() ^ param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator<<=(T param)
{
    set(get() << param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator>>=(T param)
{
    set(get() >> param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator++(void)
{
    set(get() + 1);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T> TSTTrinity<T>::operator++(int)
{
    TSTTrinity result(*this);
    
    set(get() + 1);
    return result;
}

template <typename T>
/*public */inline TSTTrinity<T>& TSTTrinity<T>::operator--(void)
{
    set(get() - 1);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T> TSTTrinity<T>::operator--(int)
{
    TSTTrinity result(*this);
    
    set(get() - 1);
    return result;
}

template <typename T>
/*public */inline TSTTrinity<T>::operator T(void) const
{
    return get();
}

template <typename T>
/*private */void TSTTrinity<T>::set(T param)
{
    _value[0] = param;
    _value[1] = param;
    _value[2] = param;
    return;
}

template <typename T>
/*private */T TSTTrinity<T>::get(void) const
{
    T result;
    
    if (_value[0] == _value[1]) {
        result = _value[0];
        _value[2] = result;
    }
    else if (_value[1] == _value[2]) {
        result = _value[1];
        _value[0] = result;
    }
    else if (_value[2] == _value[0]) {
        result = _value[2];
        _value[1] = result;
    }
    else {
        result = _value[random(3)];
        _value[0] = result;
        _value[1] = result;
        _value[2] = result;
    }
    return result;
}

template <typename T>
/*public */inline TSTTrinity<T*>::TSTTrinity(void)
{
}

template <typename T>
/*public */inline TSTTrinity<T*>::TSTTrinity(TSTTrinity const& param)
{
    set(param.get());
}

template <typename T>
/*public */inline TSTTrinity<T*>::TSTTrinity(T* param)
{
    set(param);
}

template <typename T>
/*public */inline TSTTrinity<T*>::~TSTTrinity(void)
{
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator=(TSTTrinity const& param)
{
    set(param.get());
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator=(T* param)
{
    set(param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator+=(int param)
{
    set(get() + param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator-=(int param)
{
    set(get() - param);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator++(void)
{
    set(get() + 1);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*> TSTTrinity<T*>::operator++(int)
{
    TSTTrinity result(*this);
    
    set(get() + 1);
    return result;
}

template <typename T>
/*public */inline TSTTrinity<T*>& TSTTrinity<T*>::operator--(void)
{
    set(get() - 1);
    return *this;
}

template <typename T>
/*public */inline TSTTrinity<T*> TSTTrinity<T*>::operator--(int)
{
    TSTTrinity result(*this);
    
    set(get() - 1);
    return result;
}

template <typename T>
/*public */inline T* TSTTrinity<T*>::operator->(void) const
{
    return get();
}

template <typename T>
/*public */inline TSTTrinity<T*>::operator T*(void) const
{
    return get();
}

template <typename T>
/*private */void TSTTrinity<T*>::set(T* param)
{
    _value[0] = param;
    _value[1] = param;
    _value[2] = param;
    return;
}

template <typename T>
/*private */T* TSTTrinity<T*>::get(void) const
{
    T* result;
    
    if (_value[0] == _value[1]) {
        result = _value[0];
        _value[2] = result;
    }
    else if (_value[1] == _value[2]) {
        result = _value[1];
        _value[0] = result;
    }
    else if (_value[2] == _value[0]) {
        result = _value[2];
        _value[1] = result;
    }
    else {
        result = _value[random(3)];
        _value[0] = result;
        _value[1] = result;
        _value[2] = result;
    }
    return result;
}

}// end of namespace

#endif
