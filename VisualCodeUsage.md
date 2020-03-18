**This is a short illustrated walk-through for the usage of [Visual Code](https://code.visualstudio.com) for [Remote Development using SSH](https://code.visualstudio.com/docs/remote/ssh).**

This instruction was written for Visual Studio Code version 1.43.0, 2020-03-09.



**Steps**

First, we navigate to [Visual Code](https://code.visualstudio.com) and download the application for our host operating system.

![Choose your installer from the drop-down](/MarkdownImages/VSCodeInstaller.png)

Run the installer

![Run the installer](/MarkdownImages/VSCodeInstallerRun.png)

Accept License

![Accept License](/MarkdownImages/VSCodeLicense.png)

Choose the destination for the application

![](/MarkdownImages/VSCodeDestInstall.png)

Select the start menu folder

![](/MarkdownImages/StartMenuFolder.png)

Select additional tasks - in my case I chose all for the easier work flows.

![](/MarkdownImages/SelAddTasks.png)

We get an overview of the selected settings.

![](/MarkdownImages/InstallCheck.png)

Now Visual Studio Code is ready to be installed.

![](/MarkdownImages/CompletingSetup.png)

This is what the Welcome page looks like now:

![](/MarkdownImages/VSCodeWelcome.png)

First we install the "Remote - SSH" Extension, by clicking the button on the left and searching for it.

![](/MarkdownImages/VSCodeRemoteSSH.png)

After the short installation it looks something like this.
There should appear the Remote-Tools button on the activity bar.

Also note the small green icon in the bottom left corner for connecting to a remote.

![](/MarkdownImages/RemoteSSHInstalled.png)

We click the green icon and have a box with several options pop up.

![](/MarkdownImages/ClickRemote.png)

With the second options one can edit the ssh configuration.

![](/MarkdownImages/SSHConfig.png)

For this project this is what I used:

![](/MarkdownImages/EditSSHConfig.png)

To connect to a host, we just open the Remote tools again, and choose the SSH-Connect option.

![](/MarkdownImages/SSHConnectHost.png)

We can select a host out of our configuration from the drop-down.

![](/MarkdownImages/SSHSelectHost.png)

In case there are any passwords involved in the authentication, you are prompted to type them in the console popping up at the top.

![](/MarkdownImages/SSHPasswords.png)

To have a better view at the process, one can open the output tab at the bottom. The 'Remote - SSH' selection to the right shows the ssh-output.

![](/MarkdownImages/SSHPassword2.png)

When everything is finished, the green remote button changes from 'Opening Remote...' to 'SSH: {server-name}'. 

![](/MarkdownImages/SeeSSHOutput.png)

Once everything is set up, we can open a terminal on the remote system in our editor.

![](/MarkdownImages/UseChildConsole.png)

Here is one example of a small interaction, confirming that we are working on the remote.

![](/MarkdownImages/ConsoleExample.png)

From the left button we can choose a folder to be shown in the explorer.
The folder can be interactively navigated to in the pop up.

![](/MarkdownImages/RemoteFiles.png)

From there we can open files and folders as we like. 
Files get opened in the editor.

![](/MarkdownImages/RemoteCode.png)

Opening and editing files can be supported by additional extensions to the editor. Just search the marketplace _(Ctrl+Shift+X)_ or get some recommendations based on the file-extension.
Here are the extensions used for the gpusat-project:


![](/MarkdownImages/AddExtensions.png)

After editing a file, we want to **commit** the changes to the version control system. After pressing _(Ctrl+Shift+G)_ we can enter a commit message and stage files as we like.

![](/MarkdownImages/SourceControl.png)

If there were no changes staged, we are prompted with the option to automatically **stage** all changed files. 

![](/MarkdownImages/StageChanges.png)

For most additional git operations such as push there are the three dots in the source control frame.

![](/MarkdownImages/GitOptions.png)

If there are no git credentials provided yet, they get prompted when necessary in the top bar.

![](/MarkdownImages/GitCredentials.png)

To see progress of git operations we can select the Git-option in the output.

![](/MarkdownImages/Gitoutput.png)

There are also several other output streams that can be displayed.
Here are some examples:

![](/MarkdownImages/OtherOutput.png)


I hope that your setup goes well - happy coding!

Martin Röbke













