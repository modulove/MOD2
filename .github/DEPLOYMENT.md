# Deployment Configuration Guide

This guide explains how to configure automatic deployment of firmware files to `modulove.github.io` (GitHub Pages).

## How It Works

The GitHub Actions workflow automatically:
1. Builds all 7 firmware files on every push to `main`
2. Renames them to match the `MOD2_*.uf2` naming convention
3. Pushes them to the `modulove.github.io` repository (available at `https://modulove.github.io/releases/`)

## Prerequisites

You need:
- Write access to the `modulove/modulove.github.io` repository
- A GitHub Personal Access Token (PAT) with `repo` scope (if default token doesn't work)

## Configuration Steps

### Option 1: Try Without Additional Configuration (Default)

The workflow will first try to use the default `GITHUB_TOKEN`. This should work if:
- The workflow is running in the `modulove` organization
- The repository has appropriate permissions

**Try running the workflow first** - it might just work!

### Option 2: Create a Personal Access Token (PAT)

If the default token doesn't work, create a PAT:

#### 1. Create Personal Access Token

1. Go to https://github.com/settings/tokens
2. Click **"Generate new token (classic)"**
3. Give it a descriptive name: `MOD2 Deployment to modulove.github.io`
4. Set expiration (recommendation: 90 days or 1 year)
5. Select scopes:
   - ✅ **repo** (Full control of private repositories)
     - This includes: repo:status, repo_deployment, public_repo, repo:invite, security_events
6. Click **"Generate token"**
7. **Copy the token immediately** - you won't be able to see it again!

#### 2. Add Token as Repository Secret

1. Go to the MOD2 repository: https://github.com/modulove/MOD2
2. Navigate to **Settings** → **Secrets and variables** → **Actions**
3. Click **"New repository secret"**
4. Name: `GH_PAT`
5. Value: Paste your Personal Access Token
6. Click **"Add secret"**

That's it! The workflow will now use `GH_PAT` to push firmware files to `modulove.github.io`.

## Testing Deployment

Once configured, the deployment will automatically run on the next push to `main`.

### Manual Test

1. Make a small change and commit to `main`
2. Go to **Actions** tab in GitHub
3. Click on the running workflow
4. Expand the "Deploy firmware to GitHub Pages (dl.modulove.io)" step
5. Check for success message: "✅ Firmware files deployed successfully"

### Verify Files on GitHub Pages

1. Go to https://github.com/modulove/modulove.github.io
2. Check the `releases/` directory
3. You should see all 7 firmware files:
   - `MOD2_braids.uf2`
   - `MOD2_clap.uf2`
   - `MOD2_claves.uf2`
   - `MOD2_fm_drum.uf2`
   - `MOD2_hihat.uf2`
   - `MOD2_kick.uf2`
   - `MOD2_vco.uf2`

4. Files are immediately available at:
   - https://modulove.github.io/releases/MOD2_clap.uf2
   - (and other firmware files)

Note: If you have a CNAME set up (like `dl.modulove.io`), files will also be available there once DNS propagates.

## GitHub Actions Artifacts Retention

**Important:** GitHub Actions artifacts (the files in the "Artifacts" section of workflow runs) have a retention policy:

- **Default retention**: 90 days
- **Maximum retention**: 400 days (GitHub Enterprise only)
- **Free tier**: Artifacts count against your storage quota

This means:
- ✅ Firmware files on `modulove.github.io` are **permanent** (stored in git repository)
- ⚠️ Firmware artifacts in GitHub Actions are **temporary** (90 days)

**Recommendation:** Always use the `modulove.github.io` links for long-term access.

## Why Did It Break?

If deployment was working before and suddenly broke, possible causes:

1. **Token expired** - PATs can expire (check expiration date)
2. **Repository permissions changed** - Check if workflow still has write access
3. **Default GITHUB_TOKEN permissions** - GitHub changed default permissions policy
4. **Workflow file changes** - Recent commits might have broken the deployment step
5. **Branch protection** - Branch protection rules on `modulove.github.io` might block pushes

## Troubleshooting

### Deployment is skipped

If you see "⚠️ No GitHub token available for deployment":
- The `GITHUB_TOKEN` is not accessible
- This shouldn't happen - contact GitHub support

### Permission denied / Clone failed

**Symptom:** Cannot clone or push to `modulove.github.io`

**Solutions:**
1. Create a PAT (see Option 2 above)
2. Verify you have write access to `modulove.github.io` repository
3. Check if organization requires SSO authorization for PAT

### No changes to deploy

**Symptom:** "No changes to deploy - firmware files are already up to date"

**This is normal!** It means:
- Firmware files haven't changed since last build
- No new deployment needed
- Files are already available on `dl.modulove.io`

### GitHub Pages not updating

**Symptom:** Files pushed but not appearing on `modulove.github.io`

**Solutions:**
1. Check GitHub Pages settings in `modulove.github.io` repository
2. Verify GitHub Pages is enabled and pointing to correct branch
3. Check for build errors in GitHub Pages deployment
4. Files are available immediately at `modulove.github.io`
5. Custom domains (like `dl.modulove.io`) may take 2-5 minutes for CDN cache to clear
6. Try hard refresh: Ctrl+F5 (Windows) or Cmd+Shift+R (Mac)

## Advanced Configuration

### Change Deployment Branch

By default, files are pushed to the `main` or `master` branch of `modulove.github.io`. To change this, edit the workflow file and modify:

```yaml
git push origin main || git push origin master
```

Replace with your desired branch, e.g.:
```yaml
git push origin gh-pages
```

### Change Deployment Directory

Files are deployed to `releases/` directory. To change this, edit:

```yaml
mkdir -p pages-repo/releases
cp -v ./firmware-release/*.uf2 pages-repo/releases/
```

Replace `releases/` with your desired path.

### Add Deployment Notifications

To get notified when deployments succeed/fail, add a notification step to the workflow.

## Security Best Practices

1. **Use short-lived PATs** - Set expiration to 90 days and renew regularly
2. **Limit token scope** - Only grant `repo` scope, nothing more
3. **Use repository secrets** - Never commit tokens to the repository
4. **Enable SSO** - If using organization, require SSO authorization
5. **Rotate tokens** - Change PATs every 3-6 months

## Support

If you encounter issues:
1. Check the GitHub Actions logs for detailed error messages
2. Verify repository permissions
3. Check GitHub Pages settings in `modulove.github.io`
4. Open an issue in the repository if problems persist

## Artifact Storage Limits

Free GitHub accounts have storage limits:
- **Free**: 500 MB storage, 1 GB bandwidth/month
- **Pro**: 2 GB storage, 10 GB bandwidth/month
- **Team**: 50 GB storage, 100 GB bandwidth/month

Each firmware build creates ~7-10 MB of artifacts. With 90-day retention, this is manageable, but monitor your usage at:
- https://github.com/settings/billing
