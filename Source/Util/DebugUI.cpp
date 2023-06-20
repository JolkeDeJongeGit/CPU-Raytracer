#include "precomp.h"
#include "DebugUI.h"
namespace Util
{
	void DebugUi::Print(const char* a_Text)
	{
		m_Screen->Print(a_Text, (int)m_Pos.x, (int)m_Pos.y, 0xffffff);
		m_Pos.y += 12;
	}

	void DebugUi::End()
	{
		m_Pos = m_OrgPos;
	}
}
