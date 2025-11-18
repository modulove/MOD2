# Setting Up MOD2 Downloads on modulove.github.io

Quick guide to add the MOD2 firmware download page to your GitHub Pages site.

## Files to Add

All files are located in `.github/modulove-github-io-additions/`

1. **mod2-download-page.html** - The complete download page
2. **README.md** - Detailed documentation

## Quick Setup

### Step 1: Access modulove.github.io Repository

```bash
# Clone the repository (if not already)
git clone https://github.com/modulove/modulove.github.io.git
cd modulove.github.io
```

### Step 2: Copy the Download Page

```bash
# Option A: As main page (if repository is empty)
cp /path/to/MOD2/.github/modulove-github-io-additions/mod2-download-page.html index.html

# Option B: As dedicated page
cp /path/to/MOD2/.github/modulove-github-io-additions/mod2-download-page.html mod2.html

# Option C: In subdirectory
mkdir -p mod2
cp /path/to/MOD2/.github/modulove-github-io-additions/mod2-download-page.html mod2/index.html
```

### Step 3: Ensure releases/ Directory Exists

The GitHub Actions workflow from MOD2 repository will automatically create and populate the `releases/` directory with firmware files.

```bash
# The workflow will create:
# releases/MOD2_braids.uf2
# releases/MOD2_clap.uf2
# releases/MOD2_claves.uf2
# releases/MOD2_fm_drum.uf2
# releases/MOD2_hihat.uf2
# releases/MOD2_kick.uf2
# releases/MOD2_vco.uf2
```

### Step 4: Commit and Push

```bash
git add .
git commit -m "Add MOD2 firmware download page"
git push origin main  # or 'master' depending on your default branch
```

### Step 5: Access Your Page

After GitHub Pages deploys (usually 1-2 minutes):

- **modulove.github.io**: https://modulove.github.io/mod2.html
- **With CNAME (dl.modulove.io)**: https://dl.modulove.io/mod2.html

## What the Page Includes

### 7 Direct Download Firmware
- BRAIDS, CLAP, CLAVES, FM_DRUM, HIHAT, KICK, VCO
- Click to download .uf2 files directly

### 2 Patreon-Exclusive Firmware
- **SAMPLE** - Opens modal with info and Patreon link
- **BREAK BEATS** - Opens modal with info and Patreon link

### Features
- Dark, modern design
- Responsive (mobile-friendly)
- Modal popups for Patreon firmware
- Installation instructions included
- Keyboard navigation support

## Testing the Setup

1. ✅ Visit the page URL
2. ✅ Click each direct download link (should download .uf2)
3. ✅ Click "SAMPLE" firmware (modal should appear)
4. ✅ Click "BREAK BEATS" firmware (modal should appear)
5. ✅ Test "Visit Patreon" buttons (open correct URLs)
6. ✅ Test on mobile device

## Automatic Firmware Updates

The MOD2 repository's GitHub Actions workflow automatically:
1. Builds firmware on every push to `main`
2. Pushes updated .uf2 files to `modulove.github.io/releases/`
3. No manual intervention needed!

See `MOD2/.github/DEPLOYMENT.md` for configuration details.

## Customization

To customize the page, edit `mod2-download-page.html`:

### Change Colors
Find the `<style>` section and modify:
- Primary blue: `#0066ff`
- Patreon red: `#ff424d`
- Background: `#0a0a0a`

### Add More Firmware
Copy the firmware card HTML structure and update details.

### Change Layout
Modify the CSS grid settings:
```css
grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
```

## Troubleshooting

### Downloads Don't Work
- Check that `releases/` directory exists
- Verify .uf2 files are present
- Check file permissions (should be 644)

### Patreon Links Wrong
Update URLs in the modal JavaScript:
- Sample: `https://www.patreon.com/posts/code-for-mod2-131363551`
- Break Beats: `https://www.patreon.com/posts/code-for-mod2-133952127`

### Page Not Updating
- Wait 1-2 minutes for GitHub Pages to deploy
- Hard refresh: Ctrl+F5 (Windows) or Cmd+Shift+R (Mac)
- Check GitHub Pages settings in repository

### Firmware Not Deploying from MOD2 Repo
- Check GitHub Actions in MOD2 repository
- Verify GH_PAT secret is configured
- See `MOD2/.github/DEPLOYMENT.md`

## Support

- **Page issues**: Edit in `modulove.github.io` repository
- **Firmware builds**: Check `MOD2` repository Actions tab
- **Documentation**: See `.github/modulove-github-io-additions/README.md`

## Quick Reference

| Item | Location |
|------|----------|
| Download page source | `.github/modulove-github-io-additions/mod2-download-page.html` |
| Full documentation | `.github/modulove-github-io-additions/README.md` |
| Deployment guide | `.github/DEPLOYMENT.md` |
| Live page | `https://modulove.github.io/mod2.html` |
| Firmware files | `https://modulove.github.io/releases/MOD2_*.uf2` |
