using CommunityToolkit.Mvvm.ComponentModel;

namespace Cobalt64.Editor.ViewModels;

public partial class MainViewModel : ViewModelBase
{
    [ObservableProperty] public partial string Greeting { get; set; } = "Welcome to Avalonia!";
}