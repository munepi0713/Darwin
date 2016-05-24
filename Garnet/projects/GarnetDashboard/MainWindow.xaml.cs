using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GarnetDashboard
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        private class Item
        {
            public string Generation { get; set; }
        }

        public MainWindow()
        {
            InitializeComponent();

            this.Title = "Garnet Dashboard";

            textProjectFile.Text = Properties.Settings.Default.LastProjectFile;
            if (Properties.Settings.Default.LastProjectFile.Length > 0)
            {
                textProjectDirectory.Text = System.IO.Path.GetDirectoryName(Properties.Settings.Default.LastProjectFile);
            }

            textProjectFile.AllowDrop = true;
        }

        private Dictionary<string, string> loadProject(string path)
        {
            if (System.IO.File.Exists(path))
            {
                Dictionary<string, string> ret = new Dictionary<string, string>();
                ret["File"] = path;
                ret["Directory"] = System.IO.Path.GetDirectoryName(path);
                return ret;
            }
            else
            {
                if (System.IO.Directory.Exists(path))
                {
                    string confFileName = System.IO.Path.GetFileName(path) + ".garnet_conf.txt";
                    string confFilePath = path + System.IO.Path.DirectorySeparatorChar + confFileName;
                    if (System.IO.File.Exists(confFilePath))
                    {
                        Dictionary<string, string> ret = new Dictionary<string, string>();
                        ret["File"] = confFilePath;
                        ret["Directory"] = path;
                        return ret;
                    }
                }
            }
            return null;
        }

        private void buttonRunProject_Click(object sender, RoutedEventArgs e)
        {
            runProject(textProjectDirectory.Text, System.Int32.Parse(textMaxGeneration.Text));
        }

        private void buttonViewProject_Click(object sender, RoutedEventArgs e)
        {
            if (System.IO.File.Exists(textProjectFile.Text))
            {
                System.Diagnostics.Process process = System.Diagnostics.Process.Start(textProjectFile.Text);
            }
        }

        private void listGenerations_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (System.IO.Directory.Exists(textProjectDirectory.Text))
            {
                if (listGenerations.SelectedItem != null)
                {
                    Item item = (Item)listGenerations.SelectedItem;
                    showGenerationReport(textProjectDirectory.Text, item.Generation);
                }
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Properties.Settings.Default.Save();
        }

        private void textProjectFile_Drop(object sender, DragEventArgs e)
        {
            string[] files = e.Data.GetData(DataFormats.FileDrop) as string[];
            if (files != null)
            {
                textProjectFile.Text = files[0];
            }
        }

        private void textProjectFile_PreviewDragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) || e.Data.GetDataPresent(DataFormats.Text))
            {
                e.Effects = DragDropEffects.Copy;
                e.Handled = true;
            }
            else
            {
                e.Effects = DragDropEffects.None;
                e.Handled = false;
            }
        }

        private void treeProjects_PreviewDragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) || e.Data.GetDataPresent(DataFormats.Text))
            {
                e.Effects = DragDropEffects.Copy;
                e.Handled = true;
            }
            else
            {
                e.Effects = DragDropEffects.None;
                e.Handled = false;
            }
        }

        private void treeProjects_Drop(object sender, DragEventArgs e)
        {
            string[] files = e.Data.GetData(DataFormats.FileDrop) as string[];
            if (files != null)
            {
                reloadProjects(files[0]);
            }
        }

        private void treeProjects_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)treeProjects.SelectedItem;
            if (item != null)
            {
                if (item.Tag != null)
                {
                    textProjectFile.Text = (string)item.Tag;
                    loadProject();
                }
            }
        }

        private void loadProject()
        {
            Dictionary<string, string> result = loadProject(textProjectFile.Text);
            if (result != null)
            {
                showGenerations(result["Directory"]);

                textProjectFile.Text = result["File"];
                textProjectDirectory.Text = result["Directory"];

                Properties.Settings.Default.LastProjectFile = result["File"];
            }
            else
            {
                MessageBox.Show("Invalid Project File.", "Error");
            }
        }

        private void runProject(string path, int maxGeneration)
        {
            if (System.IO.Directory.Exists(path))
            {
                System.Diagnostics.ProcessStartInfo processInfo = new System.Diagnostics.ProcessStartInfo();
                processInfo.FileName = getGarnetExecutablePath();
                processInfo.Arguments = System.String.Format(
                    "run {0} -G {1}",
                    System.IO.Path.GetFileName(path),
                    maxGeneration);
                processInfo.UseShellExecute = false;
                processInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(path);

                System.Diagnostics.Process process = null;
                try
                {
                    process = System.Diagnostics.Process.Start(processInfo);
                    //process.WaitForExit();
                }
                catch
                {
                    MessageBox.Show("Failed to run.", "Error");
                }
            }
        }

        private void reloadProjects(string path)
        {
            System.IO.DirectoryInfo dinfo = new System.IO.DirectoryInfo(path);
            if (dinfo.Exists)
            {
                treeProjects.Items.Clear();
                treeProjects.Items.Add(findProjects(dinfo.FullName));
            }
        }

        private TreeViewItem findProjects(string path)
        {
            System.IO.DirectoryInfo dinfo = new System.IO.DirectoryInfo(path);
            if (dinfo.Exists)
            {
                TreeViewItem ret = new TreeViewItem();
                ret.Header = dinfo.Name;
                ret.IsExpanded = true;
                foreach (System.IO.FileInfo finfo in dinfo.EnumerateFiles("*.garnet_conf.txt", System.IO.SearchOption.TopDirectoryOnly))
                {
                    TreeViewItem sub = new TreeViewItem();
                    ret.Header = dinfo.Name;
                    ret.Tag = dinfo.FullName;
                    ret.IsExpanded = true;
                    return ret;
                }

                foreach (System.IO.DirectoryInfo subdinfo in dinfo.EnumerateDirectories())
                {
                    TreeViewItem result = findProjects(subdinfo.FullName);
                    if (result != null)
                    {
                        ret.Items.Add(result);
                    }
                }
                return ret.Items.IsEmpty ? null : ret;
            }

            return null;
        }

        /**
         * 
         */
        private void showGenerations(string directory)
        {
            // List up generation files.
            List<Item> rows = new List<Item>();
            System.IO.DirectoryInfo projDir = new System.IO.DirectoryInfo(directory);
            System.IO.DirectoryInfo genDir = new System.IO.DirectoryInfo(directory + "/generations");
            if (genDir.Exists)
            {
                foreach (System.IO.FileInfo fileInfo in genDir.EnumerateFiles())
                {
                    System.Text.RegularExpressions.Match match = System.Text.RegularExpressions.Regex.Match(fileInfo.Name, "^([0-9]{6})\\.txt(\\.gz){0,1}$");
                    if (match.Success)
                    {
                        rows.Add(new Item { Generation = match.Groups[1].Value });
                    }
                }
            }
            else
            {
                foreach (System.IO.DirectoryInfo dirInfo in projDir.EnumerateDirectories())
                {
                    if (System.Text.RegularExpressions.Regex.IsMatch(dirInfo.Name, "^[0-9]{6}$"))
                    {
                        rows.Add(new Item { Generation = dirInfo.Name });
                    }
                }
            }
            listGenerations.ItemsSource = rows;
            listGenerations.Items.SortDescriptions.Add(new SortDescription("Generation", ListSortDirection.Descending));
        }
        private string getGarnetExecutablePath()
        {
            return textGarnetExecutablePath.Text;
        }

        private void showGenerationReport(string path, string generation)
        {
            System.Diagnostics.ProcessStartInfo processInfo = new System.Diagnostics.ProcessStartInfo();
            processInfo.FileName = getGarnetExecutablePath();
            processInfo.Arguments = System.String.Format(
                "report {0} -r test -g {1}",
                System.IO.Path.GetFileName(path),
                generation);
            processInfo.UseShellExecute = false;
            processInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(path);

            System.Diagnostics.Process process = null;
            try
            {
                //
                process = System.Diagnostics.Process.Start(processInfo);
                process.WaitForExit();

                // Find report file.
                System.IO.DirectoryInfo reportDir = new System.IO.DirectoryInfo(path + "\\reports");
                string pattern = "g" + generation + "_i*";
                foreach (System.IO.DirectoryInfo dir in reportDir.EnumerateDirectories(pattern, System.IO.SearchOption.TopDirectoryOnly))
                {
                    // Open the report.
                    System.Diagnostics.Process.Start(dir.FullName + "\\test_reports.html");
                    break;
                }
            }
            catch
            {
                MessageBox.Show("Failed to make Report.", "Error");
            }
        }

        private void showDetailReport(string path, string generation)
        {
            System.Diagnostics.ProcessStartInfo processInfo = new System.Diagnostics.ProcessStartInfo();
            processInfo.FileName = getGarnetExecutablePath();
            processInfo.Arguments = System.String.Format(
                "report {0} -r detail -g {1}",
                System.IO.Path.GetFileName(path),
                generation);
            processInfo.UseShellExecute = false;
            processInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(path);

            System.Diagnostics.Process process = null;
            try
            {
                //
                process = System.Diagnostics.Process.Start(processInfo);
                process.WaitForExit();

                // Open the report.
                System.IO.DirectoryInfo reportDir = new System.IO.DirectoryInfo(path + "\\reports");
                String filename = System.String.Format(
                    "{0}\\{1}_g{2:000000}_best_detail.html",
                    reportDir.FullName,
                    System.IO.Path.GetFileName(path),
                    generation);
                System.Diagnostics.Process.Start(filename);
            }
            catch
            {
                MessageBox.Show("Failed to make Report.", "Error");
            }
        }

        private void onDetailMenuClick(object sender, RoutedEventArgs e)
        {
            if (System.IO.Directory.Exists(textProjectDirectory.Text))
            {
                if (listGenerations.SelectedItem != null)
                {
                    Item item = (Item)listGenerations.SelectedItem;
                    showDetailReport(textProjectDirectory.Text, item.Generation);
                }
            }
        }
    }
}
