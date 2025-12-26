# Contributing to sysreport

Thank you for your interest in contributing to sysreport! This document provides guidelines for contributing to the project.

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- A clear, descriptive title
- Steps to reproduce the issue
- Expected behavior vs actual behavior
- Your system information (OS, version, hardware)
- Relevant output or error messages

### Suggesting Enhancements

Enhancement suggestions are welcome! Please open an issue with:
- A clear description of the feature
- Use cases and benefits
- Any implementation ideas you have

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Test your changes thoroughly
5. Commit with clear, descriptive messages
6. Push to your fork
7. Open a Pull Request

### Code Style

- Follow existing code style and formatting
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and modular

### Testing

Before submitting a PR:
- Compile with `make` and verify no warnings
- Test on a Linux system with various flags
- Verify the man page is accurate if you changed options
- Check that the .deb package builds with `./build-deb.sh`

### Development Setup

```bash
git clone https://github.com/YOUR_USERNAME/sysreport.git
cd sysreport
make
./build/main --help
```

### Areas for Contribution

- Additional metrics (GPU stats, battery info, etc.)
- Support for more output formats
- Performance improvements
- Documentation improvements
- Bug fixes
- Test coverage

## Code of Conduct

- Be respectful and constructive
- Welcome newcomers and help them learn
- Focus on what's best for the project and community

## Questions?

Feel free to open an issue for any questions about contributing!
