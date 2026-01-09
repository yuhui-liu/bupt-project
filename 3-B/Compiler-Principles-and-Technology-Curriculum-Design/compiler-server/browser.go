package main

import (
	"context"
	"encoding/json"
	"log"
	"net/http"
	"os"
	"os/signal"
	"time"

	"github.com/joho/godotenv"
)

// 表示请求体的结构体
type RequestData struct {
	Code string     `json:"code"`
	Mode OutputMode `json:"mode"`
}

// 表示响应体的结构体
type ResponseData struct {
	Result string `json:"result"`
}

// 表示出错时的响应体的结构体
type ErrorResponseData struct {
	Message string `json:"message"`
}

// compileHandler 处理编译请求
func compileHandler(w http.ResponseWriter, r *http.Request) {
	// 如果请求方法不是POST，返回405错误
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	defer r.Body.Close()

	// 解析请求体
	var data RequestData
	err := json.NewDecoder(r.Body).Decode(&data)
	if err != nil {
		http.Error(w, "Bad request: "+err.Error(), http.StatusBadRequest)
		return
	}

	code := data.Code
	log.Printf(`Received code:
--------------------------------------------------------------------------------
%s
--------------------------------------------------------------------------------
`, code)

	result, ok := RunTheCompiler(data.Mode, 0, code)

	if ok {
		responseData, err := json.Marshal(ResponseData{Result: result})
		if err != nil {
			http.Error(w, "Failed to generate JSON response: "+err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusOK)
		w.Write(responseData)

	} else {
		errorResponseData, err := json.Marshal(ErrorResponseData{Message: result})
		if err != nil {
			http.Error(w, "Failed to generate JSON response: "+err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusUnprocessableEntity)
		w.Write(errorResponseData)
	}
}

// corsMiddleware 处理CORS请求
func corsMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "http://localhost:3000")
		w.Header().Set("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

		if r.Method == "OPTIONS" {
			w.WriteHeader(http.StatusOK)
			return
		}

		next.ServeHTTP(w, r)
	})
}

// BrowserModeServer 启动一个HTTP服务器，处理编译请求
func BrowserModeServer() {
	// 加载环境变量
	err := godotenv.Load()
	if err != nil {
		log.Println("No .env file found.")
	}
	// 获取端口号，默认为8080
	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}
	// 创建一个新的ServeMux，并注册编译处理函数
	mux := http.NewServeMux()
	mux.HandleFunc("/compile", compileHandler)

	// 创建一个新的HTTP服务器，并设置CORS中间件
	srv := &http.Server{
		Addr:    ":" + port,
		Handler: corsMiddleware(mux),
	}

	// 启动服务器
	go func() {
		log.Printf("Starting server at :%+v\n", port)
		if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("Could not start server: %s\n", err)
		}
	}()

	// 等待中断信号以安全地关闭服务器
	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	<-c

	log.Println("Shutting down server...")

	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	if err := srv.Shutdown(ctx); err != nil {
		log.Fatalf("Server forced to shutdown: %s\n", err)
	}

	log.Println("Server exiting")

}
