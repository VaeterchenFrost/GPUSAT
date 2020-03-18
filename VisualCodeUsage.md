**This is a short illustrated walk-through for the usage of [Visual Code](https://code.visualstudio.com) for [Remote Development using SSH](https://code.visualstudio.com/docs/remote/ssh).**

This instruction was written for Visual Studio Code version 1.43.0, 2020-03-09.



**Steps**

First, we navigate to [Visual Code](https://code.visualstudio.com) and download the application for our host operating system.

![Choose your installer from the drop-down](/MarkdownImages/001VSCodeInstaller.png)

Run the installer

![Run the installer](/MarkdownImages/002VSCodeInstallerRun.png)

Accept License

![Accept License](/MarkdownImages/003VSCodeLicense.png)

Choose the destination for the application

![](/MarkdownImages/004VSCodeDestInstall.png)

Select the start menu folder

![](/MarkdownImages/005StartMenuFolder.png)

Select additional tasks - in my case I chose all for the easier work flows.

![](/MarkdownImages/006SelAddTasks.png)

We get an overview of the selected settings.

![](/MarkdownImages/007InstallCheck.png)

Now Visual Studio Code is ready to be installed.

![](/MarkdownImages/008CompletingSetup.png)

This is how the welcome window looks like today:

![](/MarkdownImages/009VSCodeWelcome.png)

First we install the "Remote - SSH" Extension, by clicking the squares-button on the left and searching for it.

![](/MarkdownImages/010VSCodeRemoteSSH.png)

After the short installation it looks something like this.
There should appear the remote-tools button on the activity bar.

Also note the small green icon in the bottom left corner for connecting to a remote via ssh.

![](/MarkdownImages/011RemoteSSHInstalled.png)

We click the green icon and have a box with several options pop up.

![](/MarkdownImages/012ClickRemote.png)

With the second options we can edit our own ssh configuration.

![](/MarkdownImages/013SSHConfig.png)

For this project this is what I used:

![](/MarkdownImages/014EditSSHConfig.png)

To connect to a host, we just open the Remote tools again, and choose the SSH-Connect option.

![](/MarkdownImages/015SSHConnectHost.png)

We can select a server out of our configuration file from the drop-down.

![](/MarkdownImages/016SSHSelectHost.png)

In case there are any passwords involved in the authentication, you are prompted to enter them in the console popping up at the top.

![](/MarkdownImages/017SSHPasswords.png)

To have a better view at the process, one can open the output tab at the bottom. The 'Remote - SSH' selection to the right shows the ssh-output.

![](/MarkdownImages/018SSHPassword2.png)

When everything is finished, the green remote button changes from 'Opening Remote...' to 'SSH: {server-name}'. 

![](/MarkdownImages/019SeeSSHOutput.png)

Once everything is set up, we can open a terminal on the remote system in our editor.

![](/MarkdownImages/020UseChildConsole.png)

Here is one example of a small interaction, confirming that we are working on the remote.

![](/MarkdownImages/021ConsoleExample.png)

From the left button we can choose a folder to be shown in the explorer.
The folder can be interactively navigated to in the pop up.

![](/MarkdownImages/022RemoteFiles.png)

From there we can open files and folders as we like. 
Files get opened in the editor.

![](/MarkdownImages/023RemoteCode.png)

Opening and editing files can be supported by additional extensions to the editor. Just search the marketplace _(Ctrl+Shift+X)_ or get some recommendations based on the file-extension.
Here are the extensions used for the gpusat-project:


![](/MarkdownImages/024AddExtensions.png)

After editing a file, we want to **commit** the changes to the version control system. After pressing _(Ctrl+Shift+G)_ we can enter a commit message and stage files as we like.

![](/MarkdownImages/025SourceControl.png)

If there were no changes staged, we are prompted with the option to automatically **stage** all changed files. 

![](/MarkdownImages/026StageChanges.png)

For most additional git operations such as push there are the three dots in the source control frame.

![](/MarkdownImages/027GitOptions.png)

If there are no git credentials provided yet, they get prompted when necessary in the top bar.

![](/MarkdownImages/028GitCredentials.png)

To see progress of git operations we can select the Git-option in the output.

![](/MarkdownImages/029Gitoutput.png)

There are also several other output streams that can be displayed.
Here are some examples:

![](/MarkdownImages/030OtherOutput.png)


I hope that your setup goes well - happy coding!

Martin Röbke













