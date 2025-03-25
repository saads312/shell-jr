# Shell Jr

### Welcome to Shell Jr!
A custom bash-style shell written in C++ with the intention of making the Command Line Interface less intimidating for new users.

![Shell Jr Banner](assets/shelljr-banner.png)

---

## Overview

Shell Jr is designed to offer a user-friendly and educational command-line experience. It comes with a clean, colored prompt, built-in commands, and even an AI-powered `explain` command to help new users understand what each command does. Whether you're a beginner or looking to polish your CLI skills, Shell Jr offers a gentle introduction to the command line.

---

## Features

- **Beginner-Friendly Interface**  
  - Custom, colored prompt displaying username, hostname, and current directory  
  - Ignores empty or whitespace-only commands  
  - Built-in tutorial (coming soon) to guide new users

- **Built-in Commands**  
  - Standard commands like `cd`, `pwd`, `echo`, `exit`, and more  
  - Special built-ins that provide extra help or insights

- **AI-powered `explain` Command**  
  - Type a command with `explain "command"` to receive a plain-English explanation  
  - Uses GPT-4o via GitHub’s AI inference endpoint for on-demand assistance

- **Command History & Autocompletion**  
  - GNU Readline integration for persistent command history  
  - Up/Down arrow navigation with plans for improved autocomplete

- **I/O Redirection Support**  
  - Support for operators like `>`, `>>`, `2>`, and `2>>`  
  - Allows users to redirect standard output and errors

- **Security & Secret Management**  
  - `.env` files are gitignored and instructions are provided to remove sensitive data from history  
  - Uses tools like BFG Repo-Cleaner to retroactively scrub secrets

---

## Getting Started

### Prerequisites

- **C++ Compiler:** Supports C++17 (e.g., g++)
- **GNU Readline:** For command history and input handling
- **libcurl:** For API requests
- **nlohmann/json:** For JSON parsing
- **Make:** To build the project

### Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/saads312/shell-jr.git
   cd shell-jr


<p align="center">
  <img src="https://github.com/user-attachments/assets/564dc5a7-0fa8-4f0c-9d16-112480bc2dbe" alt="description" width="500">
</p>
