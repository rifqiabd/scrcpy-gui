# Contributing to Qt GUI Scrcpy

First off, thank you for considering contributing to Qt GUI Scrcpy! 🎉

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include:

- **Clear title** describing the issue
- **Steps to reproduce** the behavior
- **Expected behavior** vs actual behavior
- **System information** (OS, Qt version, ADB version, scrcpy version)
- **Logs** if applicable (run with debug output)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion:

- **Use a clear title** describing the enhancement
- **Provide detailed description** of the proposed feature
- **Explain why** this enhancement would be useful
- **List alternatives** you've considered

### Pull Requests

1. Fork the repo and create your branch from `main`
2. Follow the coding standards outlined in `.cursorrules`
3. Make sure your code compiles on at least one platform
4. Update documentation if needed
5. Write clear commit messages

## Development Setup

See [DEVELOPMENT.md](docs/DEVELOPMENT.md) for detailed setup instructions.

### Quick Start
```bash
# Clone your fork
git clone https://github.com/YOUR-USERNAME/scrcpy-gui.git
cd scrcpy-gui

# Build
mkdir build && cd build
cmake ..
make
```

## Coding Standards

### C++ Style
- Follow Qt coding conventions
- Use camelCase for variables and functions
- Use PascalCase for classes
- Include guards in headers
- One class per file

### Qt Specific
- Use Qt signal/slot mechanism
- Prefer Qt containers (QList, QMap) when working with Qt APIs
- Use QObject parent-child for memory management
- Use Qt Designer (.ui files) for UI layout when possible

### Commit Messages
- Use present tense ("Add feature" not "Added feature")
- Use imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit first line to 72 characters
- Reference issues and pull requests

Example:
```
Add wireless ADB connection support

- Implement TCP/IP connection dialog
- Add device discovery via mDNS
- Update documentation

Fixes #123
```

## Testing

Currently, the project doesn't have automated tests. We welcome contributions to:
- Set up Qt Test framework
- Add unit tests for AppManager
- Add integration tests for ADB commands

## Code Review Process

1. Maintainers will review your PR
2. Address any feedback or requested changes
3. Once approved, a maintainer will merge your PR

## Community

- Be respectful and inclusive
- Help newcomers
- Give constructive feedback
- Follow the [Code of Conduct](CODE_OF_CONDUCT.md)

## Questions?

Feel free to open an issue with the `question` label.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
