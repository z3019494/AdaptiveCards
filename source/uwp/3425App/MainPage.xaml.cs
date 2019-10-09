using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using AdaptiveCards.Rendering.Uwp;
using System.Threading.Tasks;
using Windows.UI.Popups;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Shapes;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace _3425App
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        private async Task DoStuff()
        {
            for (var i = 0; i < 200; ++i)
            {
                await Task.Delay(1);
                this.CardBorder.Child = null;

                var renderedAdaptiveCard = FakeRender();

                if (renderedAdaptiveCard != null)
                {
                    this.CardBorder.Child = renderedAdaptiveCard;
                    this.CountText.Text = i.ToString();
                }

                await Task.Delay(100);
                GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced);
            }
        }

        private FrameworkElement FakeRender()
        {
            StackPanel stackPanel = new StackPanel();

            for (uint i = 0; i < 20; i++)
            {
                TextBlock textBlock = new TextBlock();
                textBlock.Text = "New TextBlock";

                stackPanel.Children.Add(textBlock);

                Grid grid = new Grid();
                stackPanel.Children.Add(grid);
            }
            return stackPanel;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            DoStuff();
        }
    }
}
