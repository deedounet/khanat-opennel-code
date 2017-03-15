# Création de l'entrée du menu.
mkdir -p $HOME/.local/share/applications/ $HOME/.local/share/khanat/

cp -r /tmp/khanat_installer/* $HOME/.local/share/khanat/

cat <<EOF>> $HOME/.local/share/applications/khanat.desktop
[Desktop Entry]
Version=1.0
Name=Khanat
Type=Application
GenericName=Game Client
Comment=Khanat client
TryExec=$HOME/.local/share/khanat/khanat_client
Icon=$HOME/.local/share/khanat/khanat_client.png
Terminal=false
Hidden=false
Categories=Game;Roleplaying;
EOF

echo ”Khanat have been succefully installed” || zenity --info --title=”Khanat installer” --text=”Khanat have been succefully installed” 
