import ROOT
import configparser
import argparse

def read_current( config ):
    config_file = configparser.ConfigParser()
    config_file.read( config )
    num_file = config_file["header"]["number_of_runs"]
    file_prefix = ""
    if config_file["header"]["use_selected_events"] == "true":
        file_prefix = "Selected_"

    output = []
    sensor_name = config_file["header"]["sensor"]
    for runIndex in range(int(num_file)):
        fileName = file_prefix + config_file["run%s"%runIndex]["file_name"]
        bias = config_file["run%s"%runIndex]["bias"]
        bias_value = int(bias.split("V")[0])
        try:
            temperature = config_file["run%s"%runIndex]["temperature"]
        except:
            temperature = -273
        tfile = ROOT.TFile.Open(fileName)
        ttree = tfile.Get("wfm")
        currentValue = 0;
        histo = ROOT.TH1D("histo", "", 100, 1, 1)
        ttree.Project("histo", "current")
        current = histo.GetMean()
        output.append( [config_file["header"]["sensor"], temperature, bias, current] )

    return output

if __name__ == "__main__":

    commandline_ArgsParser = argparse.ArgumentParser()
    #commandline_ArgsParser.add_argument("--Vmin", dest="Vmin", nargs="?", default="0", type=int, help="min bias")
    #commandline_ArgsParser.add_argument("--Vmax", dest="Vmax", nargs="?", default="1000", type=int, help="max bias")
    #commandline_ArgsParser.add_argument("--binSize", dest="bin_size", nargs="?", default="10", type=float, help="bin size")
    #commandline_ArgsParser.add_argument("--low", dest="low_b", nargs="?", default="0", type=int, help="low bound")
    #commandline_ArgsParser.add_argument("--high", dest="high_b", nargs="?", default="500", type=int, help="high bound")
    #commandline_ArgsParser.add_argument("--param", dest="param", nargs="?", default="pmax2[0]", type=str, help="parameter")
    #commandline_ArgsParser.add_argument("--legTitle", dest="legTitle", nargs="?", default="", type=str, help="legend title")
    #commandline_ArgsParser.add_argument("--xtitle", dest="xtitle", nargs="?", default="", type=str, help="x axis tilte")
    #commandline_ArgsParser.add_argument("--temp", dest="temp", nargs="?", default="-30", type=str, help="temperature")
    commandline_ArgsParser.add_argument("--configFile", dest="configFile", nargs="?", default="run_info_v08022018.ini", type=str, help="Configuration file")

    argv = commandline_ArgsParser.parse_args()

    current_data = read_current(argv.configFile)
    print("Sensor: %s"%current_data[0][0])
    print("Temp,Bias,Current[uA]")
    for item in current_data:
        print("%s,%s,%s"%(item[1], item[2],item[3]*1000.0))
