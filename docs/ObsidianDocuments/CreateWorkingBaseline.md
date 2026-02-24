# 1. Go to repo root (if not already there)
cd /path/to/piStepperMotorControl

# 2. Make sure working tree is clean
git status

# 3. Checkout the known-good commit
git checkout 24ac681

# 4. Create a proper branch from it
git checkout -b baseline-24ac681

# 5. Rename the directory (optional but recommended)
cd ..
mv piStepperMotorControl piStepperWorkingBaseline
cd piStepperWorkingBaseline

# 6. Remove old GitHub remote
git remote remove origin

# 7. Create NEW repo on GitHub:
#    Name: piStepperWorkingBaseline
#    DO NOT initialize with README
#    DO NOT add .gitignore
#    DO NOT add license

# 8. Add new remote (replace with your actual URL)
git remote add origin https://github.com/RobSteele49/piStepperWorkingBaseline.git

# 9. Push to GitHub
git push -u origin baseline-24ac681