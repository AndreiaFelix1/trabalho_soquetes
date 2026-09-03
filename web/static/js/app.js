// ============================================================
// RESOURCE SERVER
// DASHBOARD - Andreia & Paulo
// ============================================================

document.addEventListener("DOMContentLoaded", () => {

    initializeClock();
    initializeCharts();
    initializeNavigation();
    initializeRefresh();
    initializeResourceButtons();

});


// ============================================================
// CLOCK
// ============================================================

function initializeClock() {

    const clock = document.getElementById("systemClock");
    const date = document.getElementById("systemDate");

    if (!clock) {
        return;
    }

    function updateClock() {

        const now = new Date();

        clock.textContent = now.toLocaleTimeString(
            "pt-BR",
            {
                hour: "2-digit",
                minute: "2-digit",
                second: "2-digit"
            }
        );

        if (date) {

            date.textContent = now.toLocaleDateString(
                "pt-BR",
                {
                    day: "2-digit",
                    month: "2-digit",
                    year: "numeric"
                }
            );

        }
    }

    updateClock();

    setInterval(updateClock, 1000);
}


// ============================================================
// CHARTS
// ============================================================

function initializeCharts() {

    createActivityChart();

    createOperationsChart();

}


// ============================================================
// ACTIVITY CHART
// ============================================================

function createActivityChart() {

    const canvas = document.getElementById("activityChart");

    if (!canvas) {
        return;
    }

    new Chart(canvas, {

        type: "line",

        data: {

            labels: [
                "00h",
                "02h",
                "04h",
                "06h",
                "08h",
                "10h",
                "12h",
                "14h",
                "16h",
                "18h",
                "20h",
                "22h"
            ],

            datasets: [

                {
                    label: "Comandos recebidos",

                    data: [
                        2,
                        4,
                        3,
                        5,
                        8,
                        6,
                        10,
                        7,
                        12,
                        9,
                        14,
                        11
                    ],

                    borderColor: "#3b82f6",

                    backgroundColor:
                        "rgba(59,130,246,0.08)",

                    borderWidth: 2,

                    pointRadius: 2,

                    pointHoverRadius: 5,

                    tension: 0.4,

                    fill: true
                },

                {
                    label: "Comandos bem-sucedidos",

                    data: [
                        2,
                        3,
                        3,
                        4,
                        7,
                        6,
                        9,
                        7,
                        11,
                        9,
                        13,
                        10
                    ],

                    borderColor: "#22c55e",

                    backgroundColor:
                        "rgba(34,197,94,0.05)",

                    borderWidth: 2,

                    pointRadius: 2,

                    pointHoverRadius: 5,

                    tension: 0.4,

                    fill: true
                },

                {
                    label: "Comandos com erro",

                    data: [
                        0,
                        1,
                        0,
                        1,
                        1,
                        0,
                        1,
                        0,
                        1,
                        0,
                        1,
                        1
                    ],

                    borderColor: "#ef4444",

                    backgroundColor:
                        "rgba(239,68,68,0.04)",

                    borderWidth: 2,

                    pointRadius: 2,

                    pointHoverRadius: 5,

                    tension: 0.4,

                    fill: false
                }

            ]
        },

        options: {

            responsive: true,

            maintainAspectRatio: false,

            interaction: {
                intersect: false,
                mode: "index"
            },

            plugins: {

                legend: {

                    position: "bottom",

                    labels: {

                        color: "#94a3b8",

                        boxWidth: 8,

                        boxHeight: 8,

                        padding: 18,

                        font: {
                            size: 9
                        }
                    }
                },

                tooltip: {

                    backgroundColor: "#080d14",

                    borderColor: "#263447",

                    borderWidth: 1,

                    titleColor: "#f8fafc",

                    bodyColor: "#cbd5e1",

                    padding: 10
                }
            },

            scales: {

                x: {

                    grid: {
                        display: false
                    },

                    ticks: {

                        color: "#526174",

                        font: {
                            size: 8
                        }
                    }
                },

                y: {

                    beginAtZero: true,

                    ticks: {

                        color: "#526174",

                        precision: 0,

                        font: {
                            size: 8
                        }
                    },

                    grid: {

                        color:
                            "rgba(71,85,105,0.18)"
                    }
                }
            }
        }
    });
}


// ============================================================
// OPERATIONS DONUT
// ============================================================

function createOperationsChart() {

    const canvas =
        document.getElementById("operationsChart");

    if (!canvas) {
        return;
    }

    new Chart(canvas, {

        type: "doughnut",

        data: {

            labels: [
                "GET",
                "SET",
                "CREATE",
                "RESERVE",
                "RELEASE",
                "OUTROS"
            ],

            datasets: [

                {

                    data: [
                        32,
                        24,
                        18,
                        14,
                        7,
                        5
                    ],

                    backgroundColor: [

                        "#3b82f6",
                        "#8b5cf6",
                        "#22c55e",
                        "#f59e0b",
                        "#22d3ee",
                        "#475569"

                    ],

                    borderWidth: 0,

                    hoverOffset: 6
                }

            ]
        },

        options: {

            responsive: true,

            maintainAspectRatio: false,

            cutout: "68%",

            plugins: {

                legend: {
                    display: false
                },

                tooltip: {

                    backgroundColor: "#080d14",

                    borderColor: "#263447",

                    borderWidth: 1,

                    titleColor: "#f8fafc",

                    bodyColor: "#cbd5e1",

                    padding: 10,

                    callbacks: {

                        label: function(context) {

                            return (
                                " " +
                                context.label +
                                ": " +
                                context.parsed +
                                "%"
                            );

                        }
                    }
                }
            }
        }
    });
}


// ============================================================
// NAVIGATION
// ============================================================

function initializeNavigation() {

    const navItems =
        document.querySelectorAll(".nav-item");

    navItems.forEach(item => {

        item.addEventListener("click", function(event) {

            event.preventDefault();

            navItems.forEach(nav => {

                nav.classList.remove("active");

            });

            this.classList.add("active");

        });

    });
}


// ============================================================
// REFRESH
// ============================================================

function initializeRefresh() {

    const refreshButton =
        document.getElementById("refreshDashboard");

    if (!refreshButton) {
        return;
    }

    refreshButton.addEventListener(
        "click",
        () => {

            refreshButton.classList.add("rotating");

            setTimeout(() => {

                refreshButton.classList.remove(
                    "rotating"
                );

            }, 700);

        }
    );
}


// ============================================================
// NEW RESOURCE
// ============================================================

function initializeResourceButtons() {

    const button =
        document.querySelector(".primary-button");

    if (!button) {
        return;
    }

    button.addEventListener("click", () => {

        showNotification(
            "Interface pronta para criar um novo recurso."
        );

    });
}


// ============================================================
// NOTIFICATION
// ============================================================

function showNotification(message) {

    const notification =
        document.createElement("div");

    notification.className =
        "dashboard-notification";

    notification.innerHTML = `
        <span class="notification-dot"></span>
        <span>${message}</span>
    `;

    document.body.appendChild(notification);

    setTimeout(() => {

        notification.classList.add("show");

    }, 10);

    setTimeout(() => {

        notification.classList.remove("show");

        setTimeout(() => {

            notification.remove();

        }, 300);

    }, 3000);
}
