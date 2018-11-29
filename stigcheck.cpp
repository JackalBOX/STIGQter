/*
 * STIGQter - STIG fun with Qt
 *
 * Copyright © 2018 Jon Hood, http://www.hoodsecurity.com/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stigcheck.h"

QString PrintSTIGCheck(STIGCheck s)
{
    return QString::number(s.id);
}

STIGCheck::STIGCheck()
{

}

STIGCheck::STIGCheck(const STIGCheck &right) : STIGCheck()
{
    id = right.id;
    stig = right.stig;
    cci = right.cci;
    rule = right.rule;
    severity = right.severity;
    title = right.title;
    vulnDescription = right.vulnDescription;
    falsePositives = right.falsePositives;
    falseNegatives = right.falseNegatives;
    fix = right.fix;
    check = right.check;
    documentable = right.documentable;
    mitigations = right.mitigations;
    severityOverrideGuidance = right.severityOverrideGuidance;
    checkContentRef = right.checkContentRef;
    potentialImpact = right.potentialImpact;
    thirdPartyTools = right.thirdPartyTools;
    mitigationControl = right.mitigationControl;
    responsibility = right.responsibility;
}

STIGCheck::~STIGCheck()
{

}
