//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace App1;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void MainPage::buttonSignInSilent_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	textBox->Text = ref new Platform::String(L"buttonSignInSilent_Click called");

	auto task = m_user.signin_silently();

	pplx::create_task(task)
	.then([this](pplx::task<xbox::services::xbox_live_result<xbox::services::system::sign_in_result>> t)
	{
		auto result = t.get();
		std::wstringstream ss;
		ss << L"buttonSignInSilent_Click called\r\n";
		ss << L"Status: ";
		ss << result.payload().status();
		textBox->Text = ref new Platform::String(ss.str().c_str());
	});
}


void MainPage::buttonSignIn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	textBox->Text = ref new Platform::String(L"buttonSignIn_Click called");

	auto task = m_user.signin(Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher);

	pplx::create_task(task)
	.then([this](pplx::task<xbox::services::xbox_live_result<xbox::services::system::sign_in_result>> t)
	{
		auto result = t.get();
		std::wstringstream ss;
		ss << L"buttonSignIn_Click called\r\n";
		ss << L"Status: ";
		ss << result.payload().status();
		textBox->Text = ref new Platform::String(ss.str().c_str());
	});
}
