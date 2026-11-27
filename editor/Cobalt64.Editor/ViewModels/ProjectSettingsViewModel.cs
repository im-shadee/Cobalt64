using Cobalt64.Editor.Models.Types;
using CommunityToolkit.Mvvm.ComponentModel;

namespace Cobalt64.Editor.ViewModels;

public partial class ProjectSettingsViewModel : ViewModelBase
{
    [ObservableProperty] public partial string GameName { get; set; } = "DefaultGame";
    [ObservableProperty] public partial Vector2 Resolution { get; set; } = new Vector2(320f, 240f);
    [ObservableProperty] public partial byte FramebufferCount { get; set; } = 2;
    [ObservableProperty] public partial eRdpDitherMode DitherMode { get; set; } = eRdpDitherMode.None_None;
    [ObservableProperty] public partial float TargetFramerate { get; set; } = 60;
    [ObservableProperty] public partial ushort DefaultSceneIndex { get; set; } = 0;
    [ObservableProperty] public partial uint TargetMemoryUsage { get; set; } = 4_000_000;
    [ObservableProperty] public partial uint SceneDataMemory { get; set; } = 64_000;
    [ObservableProperty] public partial uint GlobalDataMemory { get; set; } = 16_000;
}

public enum eRdpDitherMode
{
    // Mapped as RBG_A mode, e.g. Square_Square => RBG = Square, Alpha = Square
    Square_Square = 0,
    Square_InvSquare,
    Square_Noise,
    Square_None,
    
    Bayer_Bayer,
    Bayer_InvBayer,
    Bayer_Noise,
    Bayer_None,
    
    Noise_Square,
    Noise_InvSquare,
    Noise_Noise,
    Noise_None,
    
    None_Bayer,
    None_InvBayer,
    None_Noise,
    None_None
}

