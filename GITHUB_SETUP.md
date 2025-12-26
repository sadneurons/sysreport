# Quick GitHub Setup

1. **Create GitHub repository:**
   - Go to https://github.com/new
   - Name: `sysreport`
   - Description: "Comprehensive Linux system monitoring utility"
   - Public/Private: Your choice
   - Don't initialize with README (we have one)

2. **Push your code:**
   ```bash
   cd /home/tenebris/Documents/cpp_projects/my_project
   git branch -M main
   git commit -m "Initial commit: sysreport v0.1.0"
   git remote add origin https://github.com/YOUR_USERNAME/sysreport.git
   git push -u origin main
   ```

3. **Create a release with the .deb file:**
   - Go to your repo → Releases → Create new release
   - Tag: `v0.1.0`
   - Title: `sysreport v0.1.0`
   - Description: Copy from README features section
   - Attach file: Upload `sysreport_0.1.0_amd64.deb`
   - Publish release

4. **Users can then install with:**
   ```bash
   wget https://github.com/YOUR_USERNAME/sysreport/releases/download/v0.1.0/sysreport_0.1.0_amd64.deb
   sudo dpkg -i sysreport_0.1.0_amd64.deb
   ```

That's it! Your package is now publicly available.
