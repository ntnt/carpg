using CrashHandler.Properties;
using FluentFTP;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;

namespace CrashHandler.Providers
{
    public class FtpProvider
    {
        public List<string> GetNewReports()
        {
            List<string> newReports = new List<string>();
            Log.Verbose("Getting new reports...");

            try
            {
                FtpClient client = new FtpClient(Settings.Default.FtpUrl);
                client.Credentials = new NetworkCredential(Settings.Default.FtpUser, Settings.Default.FtpPassword);
                client.Connect();

                List<string> items = client.GetListing("/crash_reports").Select(x => x.Name).ToList();
                Directory.CreateDirectory("reports");

                foreach (string item in items)
                {
                    string source = $"/crash_reports/{item}";
                    string path = $"reports/{item}";
                    if (File.Exists(path))
                        Log.Warning($"File {item} already exists.");
                    else if (!client.DownloadFile(path, source))
                        Log.Error($"File {item} download failed.");
                    else
                    {
                        client.DeleteFile(source);
                        newReports.Add(item);
                    }
                }
            }
            catch(Exception e)
            {
                Log.Error($"Failed to get new reports: {e}");
            }

            if (newReports.Count > 0)
                Log.Info($"Returned {newReports.Count} new reports.");
            else
                Log.Verbose("No new reports.");
            return newReports;
        }
    }
}
