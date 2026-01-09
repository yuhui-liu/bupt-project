package main

import (
	"log"

	"github.com/alecthomas/kong"
)

// CLI 结构体定义了命令行参数和选项
type CLI struct {
	Browser struct {
	} `cmd:"" help:"Run in browser mode."`
	Cli struct {
		Terminator string `help:"Set the string to end." short:"t" default:"EOF"`
	} `cmd:"" help:"Run in cli mode."`
	File struct {
		Input  string `help:"Path to input file." short:"i"`
		Output string `help:"Path to output file." short:"o"`
	} `cmd:"" help:"Run in file mode."`
	Show OutputMode `help:"Show the intermediate result. Available: 0: show token sequence, 1: show ast, others: show C code. In browser mode, the outputmode will be specified by the browser, this will be ignored." enum:"0,1,2" short:"s" default:"2"`
}

func main() {
	// 解析命令行参数
	var cli CLI
	ctx := kong.Parse(&cli, kong.Name("p2c"), kong.Description(`A simple compiler from Pascal-S to C.
 ####                                ##    ####    ###  
 #   #   ####   ####   ###    ####    #        #  #   # 
 ####   #   #  ###    #      #   #    #     ###   #     
 #      #  ##    ###  #      #  ##    #    #      #   # 
 #       ## #  ####    ###    ## #   ###   #####   ###  
`))

	if ctx.Error != nil {
		log.Fatalf("Error:%s\n", ctx.Error)
	}

	// 根据命令行参数选择模式
	switch ctx.Command() {
	case "browser":
		log.Println("You are in browser mode.")
		BrowserModeServer()
	case "cli":
		log.Println("You are in cli mode. Please input source code.")
		CliModeHandler(cli.Cli.Terminator, cli.Show)
	case "file":
		FileModeHandler(cli.File.Input, cli.File.Output, cli.Show)
	default:
		panic(ctx.Command())
	}
}
