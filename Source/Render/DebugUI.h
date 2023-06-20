#pragma once
#include <string>
namespace Util
{
	class DebugUi
	{
	public:
		DebugUi() = default;
		DebugUi(Template::Surface* a_Screen , Util::vec2 a_Pos) : m_Screen(a_Screen), m_Pos(a_Pos), m_OrgPos(a_Pos) {};
		void Print(const char* a_Text);
		template<typename T> 
		void Print(std::string a_Text, T a_Number);
		template<typename T>
		void Print(std::string a_Text, T a_Number, Util::color a_C);
		void End();
	private:
		Util::vec2 m_Pos;
		Util::vec2 m_OrgPos;
		Template::Surface* m_Screen = nullptr;
	};

	template<typename T>
	inline void DebugUi::Print(std::string a_Text, T a_Number)
	{
		std::string s = a_Text + std::to_string(a_Number);

		m_Screen->Print(s.c_str(), (int)m_Pos.x, (int)m_Pos.y, 0xffffff);
		m_Pos.y += 12;
	}

	template<typename T>
	inline void DebugUi::Print(std::string a_Text, T a_Number, Util::color a_C)
	{
		std::string s = a_Text + std::to_string(a_Number);

		m_Screen->Print(s.c_str(), (int)m_Pos.x, (int)m_Pos.y, a_C.Get());
		m_Pos.y += 12;
	}

}