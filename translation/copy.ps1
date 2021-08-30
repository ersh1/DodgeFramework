# USER DEFINED
$outDir = "D:\Skyrim Mod Organizer\mods\Dodge Framework\Interface\Translations"

$strings = @('chinese', 'czech', 'english', 'french', 'german', 'italian', 'japanese', 'polish', 'russian', 'spanish')

ForEach ($string in $strings)
{
    Copy-Item "DodgeFramework_english.txt" -Destination "$outDir\DodgeFramework_$string.txt"
}