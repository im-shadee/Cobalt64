using Cobalt64.Editor.Models.Scripts;

namespace Cobalt64Editor.Tests.Models.Scripts;

public class ScriptWriterTests
{
    private string m_TempOutputPath;
    
    [SetUp]
    public void SetUp()
    {
        // Create a unique temp path for the generated file
        m_TempOutputPath = Path.Combine(Path.GetTempPath(), $"{Guid.NewGuid()}_actual.txt");
    }
    
    [TearDown]
    public void TearDown()
    {
        // Delete the generated output file after each test
        if (File.Exists(m_TempOutputPath)) File.Delete(m_TempOutputPath);
    }
    
    [Test]
    public void SW_GetContentFor_WithH_MatchesExpectedFile()
    {
        // Build the path to the expected file in the test output directory
        string expectedPath = Path.Combine(
            TestContext.CurrentContext.TestDirectory,
            "TestFiles",
            "MyScript_expected_h.txt"
        );

        // Run your logic that creates the output file at _tempOutputPath
        string output = ScriptWriter.GetContentFor(ScriptWriter.eScriptLanguage.H, "my_script");

        // Read and compare the expected file and the output (normalize line endings to avoid Windows/Linux issues)
        string expectedContent = File.ReadAllText(expectedPath).ReplaceLineEndings();
        Assert.That(output, Is.EqualTo(expectedContent));
    }
    
    [Test]
    public void SW_GetContentFor_WithC_MatchesExpectedFile()
    {
        // Build the path to the expected file in the test output directory
        string expectedPath = Path.Combine(
            TestContext.CurrentContext.TestDirectory,
            "TestFiles",
            "MyScript_expected_c.txt"
        );

        // Run your logic that creates the output file at _tempOutputPath
        string output = ScriptWriter.GetContentFor(ScriptWriter.eScriptLanguage.C, "my_script");

        // Read and compare the expected file and the output (normalize line endings to avoid Windows/Linux issues)
        string expectedContent = File.ReadAllText(expectedPath).ReplaceLineEndings();
        Assert.That(output, Is.EqualTo(expectedContent));
    }
    
    [Test]
    public void SW_ToPascalCase_FromSnakeCase_FormatsCorrectly()
    {
        string expected = "MyScript";
        Assert.That(ScriptWriter.ToPascalCase("my_script"), Is.EqualTo(expected));
    }
    
    [Test]
    public void SW_ToPascalCase_WithDash_FormatsCorrectly()
    {
        string expected = "MyScript-Test";
        Assert.That(ScriptWriter.ToPascalCase("my_script-test"), Is.EqualTo(expected));
    }
    
    [Test]
    public void SW_ToPascalCase_FromLowerCase_FormatsCorrectly()
    {
        string expected = "Myscript";
        Assert.That(ScriptWriter.ToPascalCase("myScript"), Is.EqualTo(expected));
    }
    
    [Test]
    [TestCase("")]
    [TestCase(" ")]
    [TestCase("   \n")]
    public void SW_ToPascalCase_WithEmpty_ReturnsArg(string arg)
    {
        Assert.That(ScriptWriter.ToPascalCase(arg), Is.EqualTo(arg));
    }
}
