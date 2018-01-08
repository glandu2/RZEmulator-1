/*
  *  Copyright (C) 2017 Xijezu <http://xijezu.com/>
  *
  *  This program is free software; you can redistribute it and/or modify it
  *  under the terms of the GNU General Public License as published by the
  *  Free Software Foundation; either version 3 of the License, or (at your
  *  option) any later version.
  *
  *  This program is distributed in the hope that it will be useful, but WITHOUT
  *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  *  more details.
  *
  *  You should have received a copy of the GNU General Public License along
  *  with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#include "TerrainPropInfo.h"
#include "fstream"
#include "Util.h"

bool TerrainPropInfo::Initialize(std::string szFileName)
{
    this->Release();

    std::vector<std::string> TextLines{};
    std::string strPropNum{};
    std::string strPropName{};

    std::ifstream ifstream(("Resource/NewMap/"s + szFileName).c_str(), std::ios::in);
    std::string   row;
    while (std::getline(ifstream, row)) {
        if (!row.empty() || row[0] == ';')
            TextLines.push_back(row);
    }
    ifstream.close();

    if (TextLines.empty())
        return false;

    float fCurrentVisibleRatio = 1.0f;
    int ptype = 0;
    int rtype = 0;
    int sflag = 0;
    int line_num = 0;
    int nCurrentCategory = 0;

    for(auto s : TextLines) {
        Tokenizer lines(s, '=');
        if(lines.size() < 2)
            continue;
        if(lines[0] == "CATEGORY"s) {
            nCurrentCategory = -1;
            for(int i = 0; i < m_CategoryNames.size(); ++i) {
                if(m_CategoryNames[i] == lines[1]) {
                    nCurrentCategory = i;
                    break;
                }
            }
            if(nCurrentCategory == -1) {
                nCurrentCategory  = (int)m_CategoryNames.size();
                m_CategoryNames.emplace_back(lines[1]);
            }
        } else if(lines[0]  == "VISIBLE_RATIO"s) {
            fCurrentVisibleRatio = std::stof(lines[1]);
            if(fCurrentVisibleRatio == 0.0f)
                fCurrentVisibleRatio = 1.0f;
        } else if(lines[0] == "RENDERTYPE"s) {
            if(lines[1] == "general"s)
                rtype = 0;
            else if(lines[1] == "building"s)
                rtype = 1;
        } else if(lines[0] == "PROPNAME") {
            Tokenizer vars(lines[1], ',');
            if(vars.size() < 2)
                continue;

            strPropNum = vars[0];
            strPropName = vars[1];

            if(vars.size() == 4)
                sflag = GetShadowFlag(vars[3]) | GetShadowFlag(vars[4]);
            if(strPropName.empty())
                continue;

            int prop_num = std::stoi(strPropNum);
            if(CheckPropFileType(strPropName, ".nx3"s))
                ptype = 2;
            else if(CheckPropFileType(strPropName, "_default.naf"))
                ptype = 1;
            else if(CheckPropFileType(strPropName, ".spt"))
                ptype = 3;
            else {
                if(!CheckPropFileType(strPropName, ".cob"))
                    continue;
                ptype = 4;
            }
            SetPropInfo(line_num, (uint16)prop_num, (PropType)ptype, (RenderType)rtype, nCurrentCategory, fCurrentVisibleRatio, strPropName, sflag);
        }
    }
    return true;
}

void TerrainPropInfo::Release()
{
    m_CategoryNames.clear();
    m_pPropInfo.clear();
}

int TerrainPropInfo::GetShadowFlag(std::string str)
{
    if(str == "no_ca"s)
        return 1;
    else if(str == "dy_ca"s)
        return 2;
    else if(str == "st_ca"s)
        return 4;
    else if(str == "no_re"s)
        return 8;
    else if(str == "re"s)
        return 16;
    else
        return 9;
}

bool TerrainPropInfo::CheckPropFileType(std::string rname, std::string szTail)
{
    return rname.find(szTail) != std::string::npos;
}

void TerrainPropInfo::SetPropInfo(int nLineIndex, uint16 wPropNum, PropType propType, RenderType renderType, int nCategory, float fVisibleRatio, std::string rName, int nShadowFlag)
{
    PropInfo p{};
    p.nLineIndex = nLineIndex;
    p.Type = propType;
    p.mRenderType = renderType;
    p.nCategory = nCategory;
    p.fVisibleRatio = fVisibleRatio;
    p.strName = std::move(rName);
    p.nShadowFlag = nShadowFlag;
    m_pPropInfo[wPropNum] = p;
}